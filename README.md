# Low-Rate-TCP-DoS-Attack

## Course Code: CO300

## Assignment: #37

### Overview
Low Rate TCP-Targeted DoS is an attack which is difficult to detect as compared to the traditional DoS attacks. This project simulates a Low Rate TCP-Targeted DoS Attack in ns-3.

<hr> 

### Steps to Reproduce
Clone the repository to your local machine:
```bash
git clone git@github.com:samvid25/Low-Rate-TCP-DoS-Attack.git
cd Low-Rate-TCP-DoS-Attack
```

Then, make the `waf` file executable, and build ns-3:
```bash
sudo chmod +x waf
./waf configure
```

Once ns-3 is built, this attack can be simulated by running tcp-low-rate.cc:
```bash
./waf --run scratch/tcp-low-rate
```

The PCAP files are stored in the Low-Rate-TCP-DoS-Attack/PCAPs folder for further analysis.  

<hr>

### Procedure
* Initially, the following topology is set-up.  
[//]: # (Insert image)  
* The legitimate sender runs a TCP Bulk Send application (simulating an actual TCP transfer).
* The attacker node runs a UDP On Off application (simulating the periodic bursts).
* The receiving node just has a TCP and UDP sink running to receive the segments/datagrams.
* Once the link and node parameters are defined, and the topology is set up, the start and stop times of the applications and the frequency of the attacker bursts must be defined.
* The frequency of the attacker bursts can be varied by setting the `OnTime` and `OffTime` attributes of the OnOff Helper. The attack gives the best results at OnTime = 0.25s and OffTime = 0.75s.
* All the applications except the Bulk Send application start at time t = 0. The Bulk Send application on the sender node will start at a time equal to the OffTime of the UDP On-Off application on the attacker. This is because the UDP OnOff application is initially in the Off state. This makes sure that both the sender and attacker effectively start at the same time.
* The data rate for the attacker application must be set so that it saturates the bottleneck link in the duration that it is on (OnTime).

<hr>

### Observations
The following observations have been made for a 100 second simulation, with the following parameters:


Burst Duration | Attacker's Rate
---|---
0.15s | 12000KB/s
0.2s | 12000KB/s
0.25s | 7000KB/s

[//]: # (Image)

<hr>

## References
[1] [Aleksandar Kuzmanovic and Edward W. Knightly. “Low-Rate TCP-Targeted Denial of
Service Attacks (The Shrew vs. the Mice and Elephants)”](http://oriolrius.cat/article_fitxers/326/pdf/p75-kuzmanovic.pdf)

[2] TCP Models in ns-3

[3] [Low Rate TCP-Targeted DoS Attacks - Reproducing Network Research by Stanford](https://reproducingnetworkresearch.wordpress.com/2017/06/05/cs244-17-low-rate-tcp-dos-attacks/)
