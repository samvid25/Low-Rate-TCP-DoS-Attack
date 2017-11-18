# Low-Rate-TCP-DoS-Attack

## Course Code: CO300

## Assignment: #37

### Accompanying webpage
https://samvid25.github.io/Low-Rate-TCP-DoS-Attack

### Overview
DoS attacks can be detected by counter-DoS mechanisms such as CouldFlare. Here, we simulate a 'Low-Rate TCP targetted DoS attack' which is harder to detect as compared to the traditional DoS attacks. 

These DoS attacks make use of the TCP congestion control's retransmission timeout (RTO) functionality to stop communication between a sender and a receiver. Instead of flooding the network with traffic, as in a normal DoS attack, we 'time' these floods such that they congest the network right when the TCP sender retransmits when there is a packet loss. This will lead to the sender waiting for another RTO period, after which this flood repeats again.  

Hence by carefully timing these attack bursts, we can evade counter-DoS mechanisms, and at the same hinder communication between the TCP sender and receiver.

From the [reference paper](http://oriolrius.cat/article_fitxers/326/pdf/p75-kuzmanovic.pdf):
> Denial of Service (DoS) attacks consume resources in networks, server clusters, or end hosts, with the malicious objective of preventing or severely degrading service to legitimate users. Resources that are typically consumed in such attacks include network bandwidth, server or router CPU cycles, server interrupt processing capacity, and specific protocol data structures.  Example DoS attacks include TCP SYN attacks that consume protocol data structures on the server operating system; ICMP directed broadcasts that direct a broadcast address to send a flood of ICMP replies to a target host
thereby overwhelming it; and DNS flood attacks that use specific weaknesses in DNS protocols to generate high volumes of traffic directed at a targeted victim.
>
> Common to the above attacks is a large number of compromised machines or agents involved in the attack and a “sledge-hammer” approach of high-rate transmission of packets towards the attacked node. While potentially quite harmful, the high-rate nature of such attacks presents a statistical anomaly to network monitors such that the attack can potentially be detected, the attacker identified, and the effects of the attack mitigated.
>
> In  this  paper,  we  study  low-rate  DoS  attacks,  which  we  term “shrew attacks,” that attempt to deny bandwidth to TCP flows while sending at sufficiently low average rate to elude detection by counter-DoS mechanisms.

<hr> 

### Steps to Reproduce
This project has been successfully tested and is known to be working on ns-3.27.

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

The PCAP files are stored in the `Low-Rate-TCP-DoS-Attack/PCAPs` folder for further analysis.  

<hr>

### Experimentation
The different parameters that affect the attack can be varied by varying the macros in `scratch/tcp-low-rate.cc`.
* `BULK_SEND_MAX_BYTES`: The maximum bytes the legit sender transmits (in Bytes).
* `MAX_SIMULATION_TIME`: The duration for which the simulation runs (in Seconds).
* `ATTACKER_START`: The time at which the attacker node starts the UDP burst (in Seconds).
* `ATTACKER_RATE`: The data rate of the attacker node's UDP burst (in Kb/s).
* `ON_TIME`: The burst length of the attack burst (in Seconds).
* `BURST_PERIOD`: The burst period of the attack burst (in Seconds).
* `OFF_TIME`: The duration for which there is no attacker traffic (`= BURST_PERIOD - ON_TIME`) (in Seconds).
* `SENDER_START`: The time at which the legit sender starts transmitting data (in Seconds).

<hr>

### Procedure
* Initially, the following topology is set-up.  
<p align = "center">
<img src = "https://github.com/samvid25/Low-Rate-TCP-DoS-Attack/blob/master/docs/topology.png" alt = "Topology" />
</p>

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

<table align="center">
  <thead>
    <tr>
      <th>Burst Duration (in Seconds)</th>
      <th>Attacker's Rate (in Kb/s)</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>0.15</td>
      <td>12000</td>
    </tr>
    <tr>
      <td>0.2</td>
      <td>12000</td>
    </tr>
    <tr>
      <td>0.25</td>
      <td>7000</td>
    </tr>
  </tbody>
 </table>

<p align = "center">
<img src = "https://github.com/samvid25/Low-Rate-TCP-DoS-Attack/blob/master/docs/graph.png" alt = "Throughput" />
</p>

<hr>

## References
[1] [Aleksandar Kuzmanovic and Edward W. Knightly. “Low-Rate TCP-Targeted Denial of
Service Attacks (The Shrew vs. the Mice and Elephants)”](http://oriolrius.cat/article_fitxers/326/pdf/p75-kuzmanovic.pdf)

[2] TCP Models in ns-3

[3] [Low Rate TCP-Targeted DoS Attacks - Reproducing Network Research by Stanford](https://reproducingnetworkresearch.wordpress.com/2017/06/05/cs244-17-low-rate-tcp-dos-attacks/)
