import subprocess
import re
import csv
import argparse
from statistics import stdev
from statistics import mean

csv_suffix = ""

# Stencil specifics
stencil_binary = "./builddir/origin-llvm/benchmark/stencil"
stencil_granularities = ( ("element", 0), ("line", 1), ("matrix", 2))
stencil_datatypes = ( ("Direct", 0), ("Flat", 1), ("Grouped", 2))
stencil_compiles = 100
# measure average execution time accross subruns, so we can
# calculate the standard derivation
stencil_runs = 10
stencil_sub_runs = 2

modes = {}
modes["native"] = {"stencil" : 0}
modes["dbrew"] = {"stencil" : 1}
modes["llvm"] = {"stencil" : 3}
modes["drob"] = {"stencil" : 6}

def benchmark_stencil_ctimes(transmode, granularity, datatype, compiles):
    result = subprocess.run(args=[stencil_binary,
                                  str(transmode),
                                  str(granularity),
                                  str(datatype),
                                  str(compiles),
                                  str(0)],
                            stdout=subprocess.PIPE)
    ctimes = []

    for line in result.stdout.splitlines():
        match = re.match (".*ctime=([.0-9]*);rtime=([.0-9]*).*", str(line))
        if not match:
            continue
        ctimes.append(float(match.groups()[0]))
    return ctimes

def benchmark_stencil_codesize(transmode, granularity, datatype):
    result = subprocess.run(args=[stencil_binary,
                                  str(transmode),
                                  str(granularity),
                                  str(datatype),
                                  str(1),
                                  str(0),
                                  str(1)],
                            stdout=subprocess.PIPE)
    start = 0;
    end = 0;
    code = "";

    for line in result.stdout.splitlines():
        line = line.decode('ascii')
        match = re.match (".*(0x[0-9a-f]*):\s*(([0-9a-f][0-9a-f]\s)+)\s+.*", line)
        if not match:
            continue
        code += line + "\n";
        addr = int(match.groups()[0], 0)
        length = len(match.groups()[1]) / 3
        assert(addr > 0)
        assert(length * 3 == len(match.groups()[1]))
        assert(length > 0)
        if (start == 0 or addr < start):
            start = int(addr)
        if (end == 0 or addr + length > end):
            end = int(addr + length)
    assert(start != 0 and end != 0)
    assert(end > start);
#    print("Smallest address: " + hex(start))
#    print("Highetst address: " + hex(end))
#    print("Size: " + str(int(end - start + 1)))
    return int(end - start + 1), code;

def benchmark_stencil_rtimes(transmode, granularity, datatype, runs):
    result = subprocess.run(args=[stencil_binary,
                                  str(transmode),
                                  str(granularity),
                                  str(datatype),
                                  str(runs),
                                  str(stencil_sub_runs)],
                            stdout=subprocess.PIPE)
    rtimes = []

    for line in result.stdout.splitlines():
        match = re.match (".*ctime=([.0-9]*);rtime=([.0-9]*).*", str(line))
        if not match:
            continue
        rtimes.append(float(match.groups()[1]) / stencil_sub_runs)
    return rtimes

def benchmark_stencil():
    for granularity in stencil_granularities:
        print("Processing granularity: " + granularity[0])

        fieldnames_ctimes = ['mode', 'dbrew', 'llvm', 'drob']
        fieldnames_rtimes = ['mode', 'native', 'dbrew', 'llvm', 'drob']

        ctime_file = open("results-ctimes-" + granularity[0] + csv_suffix + ".csv", 'w', newline='')
        ctime_writer = csv.DictWriter(ctime_file, fieldnames=fieldnames_ctimes)
        ctime_writer.writeheader()
        ctime_stdev_file = open("results-ctimes-stdev-" + granularity[0] + csv_suffix + ".csv", 'w', newline='')
        ctime_stdev_writer = csv.DictWriter(ctime_stdev_file, fieldnames=fieldnames_ctimes)
        ctime_stdev_writer.writeheader()
        rtime_file = open("results-rtimes-" + granularity[0] + csv_suffix + ".csv", 'w', newline='')
        rtime_writer = csv.DictWriter(rtime_file, fieldnames=fieldnames_rtimes)
        rtime_writer.writeheader()
        rtime_stdev_file = open("results-rtimes-stdev-" + granularity[0] + csv_suffix + ".csv", 'w', newline='')
        rtime_stdev_writer = csv.DictWriter(rtime_stdev_file, fieldnames=fieldnames_rtimes)
        rtime_stdev_writer.writeheader()
        #codesize_file = open("results-codesize-" + granularity[0] + csv_suffix + ".csv", 'w', newline='')
        #codesize_writer = csv.DictWriter(codesize_file, fieldnames=fieldnames_rtimes)
        #codesize_writer.writeheader()

        for datatype in stencil_datatypes:
            print("-> datatype: " + datatype[0])

            ctimes_avg = { 'mode' : datatype[0]}
            ctimes_stdev = { 'mode' : datatype[0]}
            rtimes_avg = { 'mode' : datatype[0] }
            rtimes_stdev = { 'mode' : datatype[0]}
            #codesizes = { 'mode' : datatype[0]}
            for name, transmode in modes.items():
                print("--> transmode: " + name)

                rtimes = benchmark_stencil_rtimes(transmode["stencil"], granularity[1],
                                                  datatype[1], stencil_runs)
                assert len(rtimes) > 0
                rtimes_avg[name] = '%f' % mean(rtimes)
                rtimes_stdev[name] = '%f' % stdev(rtimes)

                #codesize,code = benchmark_stencil_codesize(transmode["stencil"], granularity[1],
                #                                      datatype[1])
                #codesizes[name] = codesize

                #code_file = open("results-code-" + granularity[0] + "-" +
                #        datatype[0] + "-" + name + csv_suffix + ".txt", 'w', newline='')
                #code_file.write(code)
                #code_file.close()

                # native has no compilation
                if name == "native":
                    continue

                ctimes = benchmark_stencil_ctimes(transmode["stencil"], granularity[1],
                                                  datatype[1], stencil_compiles)
                assert len(ctimes) > 0
                ctimes_avg[name] = '%f' % mean(ctimes)
                ctimes_stdev[name] = '%f' % stdev(ctimes)

            ctime_writer.writerow(ctimes_avg)
            ctime_stdev_writer.writerow(ctimes_stdev)
            rtime_writer.writerow(rtimes_avg)
            rtime_stdev_writer.writerow(rtimes_stdev)
            #codesize_writer.writerow(codesizes)


parser = argparse.ArgumentParser(description='Run binary code optimization benchmarks')
parser.add_argument('--stencil', action='store_true', help='run stencil benchmarks')
parser.add_argument('--suffix', type=str, help='Suffix for the csv file')
args = parser.parse_args()

if args.suffix:
    csv_suffix = "-" + args.suffix

# perform the measurements for all combinations, writing the output files
if args.stencil:
    benchmark_stencil()
