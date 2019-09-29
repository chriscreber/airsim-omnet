simple Cloud
{
    parameters:
        double propDelay @unit(s);
        int numGates;
        @display("i=misc/cloud");
    gates:
        inout g[];
}
