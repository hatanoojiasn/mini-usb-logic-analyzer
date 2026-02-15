#!/usr/bin/env python3
import argparse
import time

import usb.core
import usb.util


DEFAULT_VENDOR_ID = 0xCafe
DEFAULT_PRODUCT_ID = 0x4000
DEFAULT_INTERFACE = 0
DEFAULT_ENDPOINT = 0x81


def _find_device(vendor_id: int, product_id: int):
    device = usb.core.find(idVendor=vendor_id, idProduct=product_id)
    if device is None:
        raise RuntimeError(f"USB device {vendor_id:04x}:{product_id:04x} not found")
    return device


def _claim_interface(device, interface_number: int):
    if device.is_kernel_driver_active(interface_number):
        device.detach_kernel_driver(interface_number)
    usb.util.claim_interface(device, interface_number)


def run_capture(args: argparse.Namespace) -> None:
    device = _find_device(args.vid, args.pid)
    device.set_configuration()
    _claim_interface(device, args.interface)

    endpoint = device[0][(args.interface, 0)][args.endpoint]
    start_time = time.monotonic()
    end_time = start_time + args.seconds if args.seconds is not None else None
    remaining = args.bytes
    received = 0

    with open(args.output, "wb") as output:
        while True:
            if end_time is not None and time.monotonic() >= end_time:
                break
            if remaining is not None and remaining <= 0:
                break

            chunk = endpoint.read(args.packet_size, timeout=args.timeout)
            if not chunk:
                continue
            data = bytes(chunk)
            output.write(data)
            received += len(data)
            if remaining is not None:
                remaining -= len(data)

    elapsed = time.monotonic() - start_time
    rate = (received / elapsed) if elapsed > 0 else 0
    print(f"received={received} bytes elapsed={elapsed:.2f}s rate={rate:.1f} B/s")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Capture USB bulk samples to a file.")
    parser.add_argument("--output", required=True, help="Output file path")
    parser.add_argument("--seconds", type=float, help="Capture duration in seconds")
    parser.add_argument("--bytes", type=int, help="Capture byte count")
    parser.add_argument("--vid", type=lambda v: int(v, 16), default=DEFAULT_VENDOR_ID)
    parser.add_argument("--pid", type=lambda v: int(v, 16), default=DEFAULT_PRODUCT_ID)
    parser.add_argument("--interface", type=int, default=DEFAULT_INTERFACE)
    parser.add_argument("--endpoint", type=lambda v: int(v, 16), default=DEFAULT_ENDPOINT)
    parser.add_argument("--packet-size", type=int, default=512)
    parser.add_argument("--timeout", type=int, default=1000)
    return parser.parse_args()


def main() -> None:
    args = parse_args()
    if args.seconds is None and args.bytes is None:
        raise SystemExit("Specify --seconds or --bytes")
    run_capture(args)


if __name__ == "__main__":
    main()
