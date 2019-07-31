network net
{
    submodules:
        cloud: Cloud {
            parameters:
                propDelay = 0.1s;
                @display("p=217,160");
            gates:
                g[2];
        }
        extClient1: ExtClient {
            parameters:
                srcAddr = 0;
                destAddr = 1;
        }
        extClient2: ExtClient {
            parameters:
                srcAddr = 1;
                destAddr = 0;
        }
    connections:
        extClient1.g <--> cloud.g[0];
        extClient2.g <--> cloud.g[1];
}
