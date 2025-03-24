import dns.resolver
import time
import argparse
from concurrent.futures import ThreadPoolExecutor
import statistics
from typing import List


def make_dns_query():
    resolver = dns.resolver.Resolver()
    resolver.nameservers = ['127.0.0.1']
    resolver.port = 9595

    try:
        start_time = time.time()
        answers = resolver.resolve('example.com', 'A')
        end_time = time.time()
        return end_time - start_time
    except Exception as e:
        print(f"Query failed: {e}")
        return None


def run_benchmark(num_requests: int, num_threads: int) -> List[float]:
    times = []

    with ThreadPoolExecutor(max_workers=num_threads) as executor:
        results = list(executor.map(lambda _: make_dns_query(), range(num_requests)))

    # Filter out None values (failed queries)
    times = [t for t in results if t is not None]
    return times


def print_statistics(times: List[float], num_requests: int):
    if not times:
        print("All queries failed!")
        return

    # Calculate statistics
    avg_time = statistics.mean(times)
    median_time = statistics.median(times)
    min_time = min(times)
    max_time = max(times)

    print(f"\nBenchmark Results:")
    print(f"Total Requests: {num_requests}")
    print(f"Successful Requests: {len(times)}")
    print(f"Failed Requests: {num_requests - len(times)}")
    print(f"\nTiming Statistics (seconds):")
    print(f"Average: {avg_time:.4f}")
    print(f"Median:  {median_time:.4f}")
    print(f"Min:     {min_time:.4f}")
    print(f"Max:     {max_time:.4f}")


def main():
    parser = argparse.ArgumentParser(description='DNS Benchmark Tool')
    parser.add_argument('--requests', '-r',
                        type=int,
                        default=100,
                        help='Number of DNS requests to send (default: 100)')
    parser.add_argument('--threads', '-t',
                        type=int,
                        default=10,
                        help='Number of concurrent threads to use (default: 10)')

    args = parser.parse_args()

    print(f"Starting benchmark...")
    print(f"Requests: {args.requests}")
    print(f"Threads:  {args.threads}")
    print(f"Using DNS server at localhost:9595")

    start_time = time.time()
    times = run_benchmark(args.requests, args.threads)
    total_time = time.time() - start_time

    print_statistics(times, args.requests)
    print(f"\nTotal benchmark time: {total_time:.2f} seconds")
    if times:
        print(f"Requests per second: {len(times) / total_time:.2f}")


if __name__ == "__main__":
    main()
