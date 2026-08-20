#!/usr/bin/env python3
"""Decode WorldClock meteo archive files (meteo_YYYYMMDD.bin).

File layout (little-endian):
  Header: uint32 magic, uint16 capacity
  Records: capacity x 10 bytes (uint32 time, int16 temp x100,
           uint16 hum x10, uint16 pres x10)

Empty slots (time < 1700000000) are skipped. Sentinel values
0x7FFF / 0xFFFF mean "no data" (e.g. humidity with a BMP280).

Usage:
  python archive_reader.py meteo_20260818.bin              # table to console
  python archive_reader.py meteo_20260818.bin --utc        # timestamps in UTC
  python archive_reader.py meteo_20260818.bin --csv out.csv
  python archive_reader.py *.bin                           # several files
"""

import argparse
import csv
import glob
import struct
import sys
from datetime import datetime, timezone

MAGIC_NEW = 0x4D455446   # "METF"
MAGIC_LEGACY = 0x4D455445  # "METE"
HEADER_FMT = "<IH"
HEADER_SIZE = struct.calcsize(HEADER_FMT)
REC_FMT = "<IhHH"
REC_SIZE = struct.calcsize(REC_FMT)
MIN_VALID_TIME = 1700000000  # ~Nov 2023

TEMP_NONE = 0x7FFF
HUM_NONE = 0xFFFF
PRES_NONE = 0xFFFF


def decode_value(raw, none_sentinel, scale, decimals, suffix):
    if raw == none_sentinel:
        return None
    return round(raw / scale, decimals)


def read_records(path, use_utc):
    try:
        with open(path, "rb") as f:
            data = f.read()
    except OSError as e:
        print(f"Cannot read {path}: {e}", file=sys.stderr)
        return []

    if len(data) < HEADER_SIZE:
        print(f"{path}: file too small, skipping", file=sys.stderr)
        return []

    magic, capacity = struct.unpack_from(HEADER_FMT, data, 0)
    if magic not in (MAGIC_NEW, MAGIC_LEGACY):
        print(f"{path}: bad magic 0x{magic:08X}, skipping", file=sys.stderr)
        return []

    # Trust the header capacity, but also tolerate truncated files.
    count = min(capacity, (len(data) - HEADER_SIZE) // REC_SIZE)

    tz = timezone.utc if use_utc else None
    records = []
    for i in range(count):
        offset = HEADER_SIZE + i * REC_SIZE
        t, temp, hum, pres = struct.unpack_from(REC_FMT, data, offset)
        if t < MIN_VALID_TIME:
            continue  # empty slot

        records.append({
            "time": t,
            "datetime": datetime.fromtimestamp(t, tz).strftime(
                "%Y-%m-%d %H:%M"),
            "temp": decode_value(temp, TEMP_NONE, 100, 1, "C"),
            "hum": decode_value(hum, HUM_NONE, 10, 1, "%"),
            "pres": decode_value(pres, PRES_NONE, 10, 1, "hPa"),
        })

    records.sort(key=lambda r: r["time"])
    return records


def print_table(path, records):
    print(f"\n=== {path}: {len(records)} record(s) ===")
    print(f"{'Date/Time':<17} {'Temp C':>7} {'Hum %':>7} {'Pres hPa':>9}")
    print("-" * 44)
    for r in records:
        temp = f"{r['temp']:.1f}" if r["temp"] is not None else "--"
        hum = f"{r['hum']:.1f}" if r["hum"] is not None else "--"
        pres = f"{r['pres']:.1f}" if r["pres"] is not None else "--"
        print(f"{r['datetime']:<17} {temp:>7} {hum:>7} {pres:>9}")


def write_csv(path, records, csv_path):
    with open(csv_path, "w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        writer.writerow(["unix_time", "date_time", "temp_c", "hum_pct",
                         "pres_hpa"])
        for r in records:
            writer.writerow([r["time"], r["datetime"],
                             "" if r["temp"] is None else r["temp"],
                             "" if r["hum"] is None else r["hum"],
                             "" if r["pres"] is None else r["pres"]])
    print(f"{path}: wrote {len(records)} record(s) to {csv_path}")


def main():
    parser = argparse.ArgumentParser(
        description="Decode WorldClock meteo archive files")
    parser.add_argument("files", nargs="+",
                        help="archive file(s), e.g. meteo_20260818.bin")
    parser.add_argument("--csv", metavar="FILE",
                        help="write records to a CSV file instead of "
                             "printing (only for a single input file)")
    parser.add_argument("--utc", action="store_true",
                        help="show timestamps in UTC instead of local time")
    args = parser.parse_args()

    paths = []
    for pattern in args.files:
        expanded = glob.glob(pattern)
        paths.extend(expanded if expanded else [pattern])

    if args.csv and len(paths) > 1:
        parser.error("--csv works with a single input file only")

    total = 0
    for path in paths:
        records = read_records(path, args.utc)
        total += len(records)
        if args.csv:
            write_csv(path, records, args.csv)
        else:
            print_table(path, records)

    if not total:
        print("No valid records found.", file=sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    main()
