#! /usr/bin/bash

polymer --simtype collapse_polymer --dt 1e-4 --damping .01 --initial-temp 1e-2 --writestep 10000 --in input/ --infile crw_99.txt --out output/cfmag/crw_99_cf.01_ --cf-mag .01
polymer --simtype collapse_polymer --dt 1e-4 --damping .01 --initial-temp 1e-2 --writestep 10000 --in input/ --infile crw_99.txt --out output/cfmag/crw_99_cf.02_ --cf-mag .02
polymer --simtype collapse_polymer --dt 1e-4 --damping .01 --initial-temp 1e-2 --writestep 10000 --in input/ --infile crw_99.txt --out output/cfmag/crw_99_cf.05_ --cf-mag .05
polymer --simtype collapse_polymer --dt 1e-4 --damping .01 --initial-temp 1e-2 --writestep 10000 --in input/ --infile crw_99.txt --out output/cfmag/crw_99_cf.1_ --cf-mag .1
polymer --simtype collapse_polymer --dt 1e-4 --damping .01 --initial-temp 1e-2 --writestep 10000 --in input/ --infile crw_99.txt --out output/cfmag/crw_99_cf.2_ --cf-mag .2
polymer --simtype collapse_polymer --dt 1e-4 --damping .01 --initial-temp 1e-2 --writestep 10000 --in input/ --infile crw_99.txt --out output/cfmag/crw_99_cf.5_ --cf-mag .5
polymer --simtype collapse_polymer --dt 1e-4 --damping .01 --initial-temp 1e-2 --writestep 10000 --in input/ --infile crw_99.txt --out output/cfmag/crw_99_cf1_ --cf-mag 1