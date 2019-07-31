class Packet:

    # define a packet
    def __init__(self, Speed, Gear, PX, PY, PZ, OW, OX, OY, OZ):
        self.Speed = Speed
        self.Gear = Gear
        self.PX = PX
        self.PY = PY
        self.PZ = PZ
        self.OW = OW
        self.OX = OX
        self.OY = OY
        self.OZ = OZ

    def getSpeed(self):
        return self.Speed

    def getGear(self):
        return self.Gear

    def getPX(self):
        return self.PX

    def getPY(self):
        return self.PY

    def getPZ(self):
        return self.PZ

    def getOW(self):
        return self.OW

    def getOX(self):
        return self.OX

    def getOY(self):
        return self.OY

    def getOZ(self):
        return self.OZ

# method for parsing a string and creating a Packet
def createPacket(input):
    # check if we are working with a string
    if not isinstance(input, str):
        raise TypeError
    # parse the string, there is no error checking for correct formatting
    else:
        output = []
        arr = input.split(',')
        for elem in arr:
            output.append(float(elem.split(':')[1]))
        return Packet(output[0], output[1], output[2], output[3], output[4], output[5], output[6], output[7], output[8])


createPacket("Speed:-01,Gear:-01,PX:-00108.277985,PY:-00099.679825,PZ:-00000.238537,OW:-0.974916,OX:-0.000463,OY:-0.002153,OZ:-0.222561\r")