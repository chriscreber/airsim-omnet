network net
{	parameters:
    	int numClients @prompt("Number of clients:") = default(2); 
    submodules:
        cloud: Cloud {
            parameters:
                propDelay = 0.01s;
                numGates = numClients;
                @display("p=217,160");
            gates:
                g[numClients];
        }
        extClient[numClients]: ExtClient {
            parameters:
                srcAddr = index;
        }
    connections:
        extClient[i].g <--> cloud.g[i] for i=0..numClients-1;
}
