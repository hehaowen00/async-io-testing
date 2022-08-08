# async-io-testing

## benchmark setup

- ryzen 5600x
- ubuntu linux in WSL2

## epoll

### Static Response
```
 $ wrk -t12 -c500 -d10 --latency  http://127.0.0.1:8080/
Running 10s test @ http://127.0.0.1:8080/
  12 threads and 500 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     1.85ms    3.73ms  44.82ms   88.58%
    Req/Sec   109.61k    21.25k  205.36k    75.38%
  Latency Distribution
     50%  130.00us
     75%    1.69ms
     90%    6.30ms
     99%   17.84ms
  13149885 requests in 10.10s, 1.24GB read
Requests/sec: 1302078.39
Transfer/sec:    125.42MB
```

### Sendfile
```
 $ wrk -t12 -c500 -d10 --latency  http://127.0.0.1:8080/
Running 10s test @ http://127.0.0.1:8080/
  12 threads and 500 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency    10.42ms   16.26ms 129.88ms   81.54%
    Req/Sec    57.11k    11.16k   91.98k    73.39%
  Latency Distribution
     50%  231.00us
     75%   17.43ms
     90%   38.93ms
     99%   57.00ms
  6849501 requests in 10.08s, 11.48GB read
Requests/sec: 679570.55
Transfer/sec:      1.14GB
```
