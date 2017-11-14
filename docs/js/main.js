paper.install(window);

var startAnimation;
var resetAnimation;
var refreshAnimation;

window.onload = function() {
    // Setup directly from canvas id:
    var maxX = document.getElementById('myCanvas').offsetWidth;
    var maxY = document.getElementById('myCanvas').offsetHeight;

    paper.setup('myCanvas');

    var nodeRadius = 25;
    var yLine = 50;
    var yGap = 200;

    var xc1 = 50;
    var cpoint1 = new Point(xc1, yLine);
    var node = new Path.Circle(cpoint1, nodeRadius);
    var text = new PointText(cpoint1);
    text.justification = 'center';
    text.fillColor = 'black';
    text.content = 'Node 0\nAlice';
    var node1 = node; // Sender Node
    //node1.fillColor = 'black';
    node1.strokeColor = 'black';

    var xc2 = maxX - 50;
    var cpoint2 = new Point(xc2, yLine)
    var node2 = node.clone();
    node2.position = cpoint2;
    text = new PointText(cpoint2);
    text.justification = 'center';
    text.fillColor = 'black';
    text.content = 'Node 4\nBob'; // Receiver Node
    //node2.fillColor = 'red';
    node2.strokeColor = 'black';

    var xc3 = maxX * 0.4;
    var cpoint3 = new Point(xc3, yLine);
    var node3 = node.clone();
    node3.position = cpoint3;
    text = new PointText(cpoint3);
    text.justification = 'center';
    text.fillColor = 'black';
    text.content = 'Node 2\nRouter';
    //node3.fillColor = 'yellow';
    node3.strokeColor = 'black';

    var xc4 = maxX * 0.6;
    var cpoint4 = new Point(xc4, yLine);
    var node4 = node.clone();
    node4.position = cpoint4;
    text = new PointText(cpoint4);
    text.justification = 'center';
    text.fillColor = 'black';
    text.content = 'Node 3\nRouter';
    //node4.fillColor = 'green';
    node4.strokeColor = 'black';

    var node5 = node.clone();
    var cpoint5 = new Point(xc3, yLine + yGap);
    node5.position = new Point(xc3, yLine + yGap);
    text = new PointText(cpoint5);
    text.justification = 'center';
    text.fillColor = 'black';
    text.content = 'Node 1\nEve';
    //node5.fillColor = 'blue';
    node5.strokeColor = 'black';

    var link1 = new Path.Rectangle(new Point(xc1 + nodeRadius, yLine - 2), new Point(xc3 - nodeRadius, yLine + 2));
    var textPoint = new Point((xc1 + xc3) / 2, yLine - 10);
    text = new PointText(textPoint);
    text.justification = 'center';
    text.fillColor = 'black';
    text.content = '100 Mbps';
    link1.strokeColor = 'black';
    link1.strokeWidth = 0.5;

    var link2 = new Path.Rectangle(new Point(xc3 + nodeRadius, yLine - 2), new Point(xc4 - nodeRadius, yLine + 2));
    textPoint = new Point((xc3 + xc4) / 2, yLine - 10);
    text = new PointText(textPoint);
    text.justification = 'center';
    text.fillColor = 'black';
    text.content = '1.5 Mbps';
    textPoint = new Point((xc3 + xc4) / 2, yLine + 15);
    text = new PointText(textPoint);
    text.justification = 'center';
    text.fillColor = 'black';
    text.content = 'Bottleneck Link';
    link2.strokeColor = 'black';
    link2.strokeWidth = 0.5;

    var link3 = new Path.Rectangle(new Point(xc4 + nodeRadius, yLine - 2), new Point(xc2 - nodeRadius, yLine + 2));
    textPoint = new Point((xc4 + xc2) / 2, yLine - 10);
    text = new PointText(textPoint);
    text.justification = 'center';
    text.fillColor = 'black';
    text.content = '100 Mbps';
    link3.strokeColor = 'black';
    link3.strokeWidth = 0.5;

    var link4 = new Path.Rectangle(new Point(xc3 - 2, yLine + nodeRadius), new Point(xc3 + 2, yLine + yGap - nodeRadius));
    textPoint = new Point((xc3 + 10), (yLine * 2 + yGap) / 2);
    text = new PointText(textPoint);
    text.justification = 'center';
    text.fillColor = 'black';
    text.content = '100 Mbps';
    text.rotate(90);
    link4.strokeColor = 'black';
    link4.strokeWidth = 0.5;

    var tcpPacketLength = 20;
    var point = new Point(xc1 + nodeRadius, yLine - 2);
    var size = new Size(tcpPacketLength, 4);
    var tcpPacket = new Shape.Rectangle(point, size);
    tcpPacket.fillColor = 'green';

    var tcpStart = 0;
    var tcpSent = false;
    var tcpSpeed = 10;
    var currentRTO = 1;

    var scale = 5;
    var burstTime = 0.5;
    var burstPeriod = 1;
    var burstDelay = 0.05;

    var link1Length = xc3 - xc1 - (2 * nodeRadius);

    refreshAnimation = function() {
        location.reload();
    }

    resetAnimation = function() {
        $('#startButton').removeAttr('disabled');
        paper.view.onFrame = function(event) {}
    }

    startAnimation = function() {

        $('#timeDiv').show();
        $('#resetButton').show();
        scale = parseInt($('#speed').val());
        burstTime = parseFloat($('#burstLength').val());
        //console.log(scale);

        $('#speed').attr('disabled', 'disabled');
        $('#burstLength').attr('disabled', 'disabled');

        $('#startButton').text('Resume');

        paper.view.onFrame = function(event) {
            // Each frame, rotate the path by 3 degrees:

            event.time /= scale;

            document.getElementById('timeDisplay').innerHTML = event.time.toFixed(2);
            document.getElementById('rtoDisplay').innerHTML = currentRTO / 2;

            if (tcpStart == Math.floor(event.time) && tcpSent == false) {
                tcpSent = true;
                currentRTO *= 2;
                if (tcpStart == 0) {
                    tcpStart = 1;
                } else {
                    tcpStart = tcpStart * 2 + 1;
                }
            }

            if (tcpPacket.position.x < xc3 - nodeRadius && tcpSent) {
                tcpPacket.position.x += (link1Length) / (tcpSpeed * scale);
            } else {
                tcpPacket.position.x = xc1 + nodeRadius + tcpPacketLength / 2;
                tcpSent = false;
            }


            if (event.time % (burstPeriod) >= 0 && event.time % (burstPeriod) <= (burstTime)) {
                link4.fillColor = 'red';
                if (event.time % (burstPeriod) > (burstDelay)) {
                    link2.fillColor = 'red';
                    if (event.time % (burstPeriod) > (burstDelay * 2)) {
                        link3.fillColor = 'red';
                    }
                }
            } else {
                link4.fillColor = 'white';
                if (event.time % (burstPeriod) >= (burstDelay + burstTime)) {
                    link2.fillColor = 'white';
                    if (event.time % (burstPeriod) >= (burstDelay * 2 + burstTime)) {
                        link3.fillColor = 'white';
                    }
                }
            }
        }
    }
}