#! /usr/bin/bash

polymer --simtype collapse_polymer --dt 1e-4 --damping .1 --initial-temp 1e-5 --writestep 100000 --in input/ --infile crw_100.txt --out output/cfmag/crw_100_cf.01_ --cf-mag .01
polymer --simtype collapse_polymer --dt 1e-4 --damping .1 --initial-temp 1e-5 --writestep 100000 --in input/ --infile crw_100.txt --out output/cfmag/crw_100_cf.02_ --cf-mag .02
polymer --simtype collapse_polymer --dt 1e-4 --damping .1 --initial-temp 1e-5 --writestep 100000 --in input/ --infile crw_100.txt --out output/cfmag/crw_100_cf.05_ --cf-mag .05
polymer --simtype collapse_polymer --dt 1e-4 --damping .1 --initial-temp 1e-5 --writestep 100000 --in input/ --infile crw_100.txt --out output/cfmag/crw_100_cf.1_ --cf-mag .1
polymer --simtype collapse_polymer --dt 1e-4 --damping .1 --initial-temp 1e-5 --writestep 100000 --in input/ --infile crw_100.txt --out output/cfmag/crw_100_cf.2_ --cf-mag .2
polymer --simtype collapse_polymer --dt 1e-4 --damping .1 --initial-temp 1e-5 --writestep 100000 --in input/ --infile crw_100.txt --out output/cfmag/crw_100_cf.5_ --cf-mag .5
polymer --simtype collapse_polymer --dt 1e-4 --damping .1 --initial-temp 1e-5 --writestep 100000 --in input/ --infile crw_100.txt --out output/cfmag/crw_100_cf1_ --cf-mag 1