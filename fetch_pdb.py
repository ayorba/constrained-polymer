#!/usr/bin/env python3
"""Download protein structures from RCSB PDB."""

import argparse
import json
import os
import random
import sys

import requests

SEARCH_URL   = "https://search.rcsb.org/rcsbsearch/v2/query"
DOWNLOAD_URL = "https://files.rcsb.org/download/{pdb_id}.{fmt}"
ALL_IDS_URL  = "https://data.rcsb.org/rest/v1/holdings/current/entry_ids"
GRAPHQL_URL  = "https://data.rcsb.org/graphql"

_METADATA_QUERY = """
query($ids: [String!]!) {
  entries(entry_ids: $ids) {
    rcsb_id
    struct { title }
    exptl { method }
    rcsb_entry_info {
      resolution_combined
      deposited_polymer_monomer_count
      deposited_polymer_entity_instance_count
    }
    polymer_entities {
      entity_poly { rcsb_entity_polymer_type }
      rcsb_entity_source_organism { ncbi_scientific_name }
    }
  }
}
"""

# Shorthand aliases for common experimental methods
METHODS = {
    "xray":    "X-RAY DIFFRACTION",
    "em":      "ELECTRON MICROSCOPY",
    "nmr":     "SOLUTION NMR",
    "neutron": "NEUTRON DIFFRACTION",
}


def _build_query(args, rows):
    """Assemble an RCSB Search API v2 JSON payload from parsed args."""
    nodes = []

    if args.resolution is not None:
        nodes.append({
            "type": "terminal", "service": "text",
            "parameters": {
                "attribute": "rcsb_entry_info.resolution_combined",
                "operator": "less_or_equal",
                "value": args.resolution,
            },
        })

    if getattr(args, "method", None):
        method_val = METHODS.get(args.method.lower(), args.method.upper())
        nodes.append({
            "type": "terminal", "service": "text",
            "parameters": {
                "attribute": "exptl.method",
                "operator": "exact_match",
                "value": method_val,
            },
        })

    if getattr(args, "organism", None):
        nodes.append({
            "type": "terminal", "service": "text",
            "parameters": {
                "attribute": "rcsb_entity_source_organism.ncbi_scientific_name",
                "operator": "contains_words",
                "value": args.organism,
            },
        })

    if getattr(args, "min_length", None) is not None:
        nodes.append({
            "type": "terminal", "service": "text",
            "parameters": {
                "attribute": "rcsb_entry_info.deposited_polymer_monomer_count",
                "operator": "greater_or_equal",
                "value": args.min_length,
            },
        })

    if getattr(args, "max_length", None) is not None:
        nodes.append({
            "type": "terminal", "service": "text",
            "parameters": {
                "attribute": "rcsb_entry_info.deposited_polymer_monomer_count",
                "operator": "less_or_equal",
                "value": args.max_length,
            },
        })

    if getattr(args, "protein_only", False):
        nodes.append({
            "type": "terminal", "service": "text",
            "parameters": {
                "attribute": "entity_poly.rcsb_entity_polymer_type",
                "operator": "exact_match",
                "value": "Protein",
            },
        })

    if not nodes:
        query = {"type": "terminal", "service": "full_text", "parameters": {"value": "*"}}
    elif len(nodes) == 1:
        query = nodes[0]
    else:
        query = {"type": "group", "logical_operator": "and", "nodes": nodes}

    return {
        "query": query,
        "return_type": "entry",
        "request_options": {
            "paginate": {"start": 0, "rows": rows},
        },
    }


def search(args, rows=100):
    payload = _build_query(args, rows=rows)
    resp = requests.post(SEARCH_URL, json=payload, timeout=30)
    if resp.status_code == 204:
        return []
    resp.raise_for_status()
    return [hit["identifier"] for hit in resp.json().get("result_set", [])]


def all_pdb_ids():
    resp = requests.get(ALL_IDS_URL, timeout=60)
    resp.raise_for_status()
    return resp.json()


def filter_local(entries, args):
    """Return PDB IDs from a loaded metadata list that match the current filters."""
    method_val = METHODS.get((getattr(args, "method", None) or "").lower(),
                             (getattr(args, "method", None) or "").upper())
    ids = []
    for e in entries:
        info = e.get("rcsb_entry_info") or {}
        entities = e.get("polymer_entities") or []

        if args.resolution is not None:
            res = info.get("resolution_combined") or []
            if not res or res[0] > args.resolution:
                continue

        if getattr(args, "method", None):
            methods = [x.get("method", "") for x in (e.get("exptl") or [])]
            if method_val not in methods:
                continue

        if getattr(args, "organism", None):
            words = args.organism.lower().split()
            organisms = [
                o.get("ncbi_scientific_name", "").lower()
                for pe in entities
                for o in (pe.get("rcsb_entity_source_organism") or [])
            ]
            if not any(all(w in org for w in words) for org in organisms):
                continue

        count = info.get("deposited_polymer_monomer_count") or 0
        if getattr(args, "min_length", None) is not None and count < args.min_length:
            continue
        if getattr(args, "max_length", None) is not None and count > args.max_length:
            continue

        if getattr(args, "protein_only", False):
            types = [pe.get("entity_poly", {}).get("rcsb_entity_polymer_type") for pe in entities]
            if "Protein" not in types:
                continue

        ids.append(e["rcsb_id"])
    return ids


def load_local(outdir):
    path = os.path.join(outdir, "entries.json")
    if not os.path.exists(path):
        sys.exit(f"Local metadata not found at '{path}'. Run --get-metadata first.")
    with open(path) as f:
        return json.load(f)


def has_filters(args):
    return any([
        args.resolution is not None,
        getattr(args, "method", None),
        getattr(args, "organism", None),
        getattr(args, "min_length", None) is not None,
        getattr(args, "max_length", None) is not None,
        getattr(args, "protein_only", False),
    ])


def fetch_metadata(ids, batch_size=100):
    results = []
    total = len(ids)
    for i in range(0, total, batch_size):
        batch = ids[i:i + batch_size]
        resp = requests.post(GRAPHQL_URL,
            json={"query": _METADATA_QUERY, "variables": {"ids": batch}},
            timeout=60)
        resp.raise_for_status()
        results.extend(resp.json().get("data", {}).get("entries") or [])
        print(f"  {min(i + batch_size, total)}/{total} fetched", end="\r", flush=True)
    print()
    return results


def download_one(pdb_id, fmt, outdir):
    url  = DOWNLOAD_URL.format(pdb_id=pdb_id.lower(), fmt=fmt)
    resp = requests.get(url, timeout=60)
    if resp.status_code == 404:
        print(f"  {pdb_id}: not found (404), skipping")
        return False
    resp.raise_for_status()
    path = os.path.join(outdir, f"{pdb_id.lower()}.{fmt}")
    with open(path, "wb") as f:
        f.write(resp.content)
    print(f"  {pdb_id} → {path}")
    return True


def download_all(ids, fmt, outdir):
    os.makedirs(outdir, exist_ok=True)
    ok = 0
    for pdb_id in ids:
        try:
            if download_one(pdb_id, fmt, outdir):
                ok += 1
        except requests.RequestException as e:
            print(f"  {pdb_id}: network error — {e}")
    print(f"\nDownloaded {ok}/{len(ids)} structures to '{outdir}'")


def build_parser():
    parser = argparse.ArgumentParser(
        prog="fetch_pdb.py",
        description="Download protein structures from RCSB PDB.",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""\
examples:
  %(prog)s --id 1ABC
  %(prog)s --id 1ABC 2DEF 3GHI --format cif
  %(prog)s --random
  %(prog)s --random-n 10 --resolution 2.5 --protein-only
  %(prog)s --search --organism "Homo sapiens" --method xray --limit 50
  %(prog)s --search --min-length 100 --max-length 300 --outdir ./structures
  %(prog)s --get-metadata --outdir ./structures
  %(prog)s --get-metadata --organism "Homo sapiens" --method xray --outdir ./structures
""",
    )

    mode = parser.add_mutually_exclusive_group(required=True)
    mode.add_argument(
        "--id", nargs="+", metavar="PDB_ID",
        help="Download one or more specific structures by PDB ID",
    )
    mode.add_argument(
        "--random", action="store_true",
        help="Download one random structure (filters apply)",
    )
    mode.add_argument(
        "--random-n", type=int, metavar="N",
        help="Download N random structures (filters apply)",
    )
    mode.add_argument(
        "--search", action="store_true",
        help="Download all structures matching the filter options (see --limit)",
    )
    mode.add_argument(
        "--get-metadata", action="store_true",
        help="Fetch metadata for all PDB entries (or filtered subset) and save to entries.json",
    )

    filt = parser.add_argument_group(
        "filters",
        "Combinable with --random, --random-n, and --search. Ignored with --id.",
    )
    filt.add_argument(
        "--resolution", type=float, metavar="Å",
        help="Maximum resolution in Å, e.g. 2.5 (X-ray / EM structures)",
    )
    filt.add_argument(
        "--method", metavar="METHOD",
        help="Experimental method: xray, em, nmr, neutron (or full RCSB string)",
    )
    filt.add_argument(
        "--organism", metavar="NAME",
        help='Source organism scientific name, e.g. "Homo sapiens"',
    )
    filt.add_argument(
        "--min-length", type=int, metavar="N",
        help="Minimum total deposited residue count",
    )
    filt.add_argument(
        "--max-length", type=int, metavar="N",
        help="Maximum total deposited residue count",
    )
    filt.add_argument(
        "--protein-only", action="store_true",
        help="Restrict to entries that contain at least one protein chain",
    )

    parser.add_argument(
        "--local", action="store_true",
        help="Use cached entries.json in --outdir instead of querying the RCSB API",
    )
    parser.add_argument(
        "--format", choices=["pdb", "cif"], default="pdb",
        help="File format (default: pdb)",
    )
    parser.add_argument(
        "--outdir", default=".", metavar="DIR",
        help="Output directory (default: current directory)",
    )
    parser.add_argument(
        "--limit", type=int, default=100, metavar="N",
        help="Maximum number of results returned by --search (default: 100)",
    )

    return parser


def main():
    parser = build_parser()
    args   = parser.parse_args()

    def get_ids(rows=500):
        """Return matching IDs from local cache or RCSB API."""
        if args.local:
            entries = load_local(args.outdir)
            return filter_local(entries, args) if has_filters(args) else [e["rcsb_id"] for e in entries]
        return search(args, rows=rows) if has_filters(args) else all_pdb_ids()

    try:
        if args.id:
            download_all(args.id, args.format, args.outdir)

        elif args.random:
            ids = get_ids(rows=500)
            if not ids:
                sys.exit("No structures match the given filters.")
            download_all([random.choice(ids)], args.format, args.outdir)

        elif args.random_n is not None:
            n = args.random_n
            if n < 1:
                sys.exit("--random-n must be at least 1.")
            ids = get_ids(rows=max(n * 10, 500))
            if not ids:
                sys.exit("No structures match the given filters.")
            download_all(random.sample(ids, min(n, len(ids))), args.format, args.outdir)

        elif args.get_metadata:
            if args.local:
                sys.exit("--get-metadata and --local cannot be combined.")
            ids = search(args, rows=args.limit) if has_filters(args) else (
                print("Fetching all PDB entry IDs...") or all_pdb_ids()
            )
            if not ids:
                sys.exit("No entries found.")
            print(f"Fetching metadata for {len(ids)} entries (this may take a while)...")
            entries = fetch_metadata(ids)
            os.makedirs(args.outdir, exist_ok=True)
            path = os.path.join(args.outdir, "entries.json")
            with open(path, "w") as f:
                json.dump(entries, f, indent=2)
            print(f"Saved {len(entries)} entries to '{path}'")

        elif args.search:
            if not has_filters(args) and not args.local:
                sys.exit(
                    "--search requires at least one filter "
                    "(--resolution, --method, --organism, --min-length, --max-length, --protein-only).\n"
                    "To download random structures without filters, use --random or --random-n."
                )
            ids = get_ids(rows=args.limit)
            if not ids:
                sys.exit("No structures match the given filters.")
            print(f"Found {len(ids)} structure(s).")
            download_all(ids, args.format, args.outdir)

    except requests.RequestException as e:
        sys.exit(f"Network error: {e}")


if __name__ == "__main__":
    main()
