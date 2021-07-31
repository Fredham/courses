# 多线程，1.接受线程  2.发送线程（广播）   3.算法（dijkstra_thread）
import threading
# socket编程
import socket
# sleep（实现间隔）
import time
# 读取参数
import sys
# 打包和解包数据
import pickle

ROUTE_UPDATE_INTERVAL = 30
UPDATE_INTERVAL = 1
ipaddress = "localhost"
# 字典（key->value）
allNodes = {}


class lsrNeighbour:
    def __init__(self, neighbourName, cost, neighbourPortNum):
        self.name = neighbourName
        self.cost = cost
        self.portNum = neighbourPortNum


class lsrNode:
    def __init__(self, nodeName, portNum):
        self.nodeName = nodeName
        self.portNum = portNum
        self.neighbours = []


class broadcastMessage:
    def __init__(self, source):
        self.sourceNode = source
        self.nodesReceived = [source]
        self.neighbours = []


def broadcast_thread():
    sendSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)  # ipv4 and UDP
    broadcast = broadcastMessage(currNode.nodeName)
    for neighbour in currNode.neighbours:
        broadcast.neighbours.append(neighbour)
    packet = pickle.dumps(broadcast)
    while True:
        for neighbour in currNode.neighbours:
            if (neighbour.name not in broadcast.nodesReceived):
                # print(neighbour.portNum)

                # 把neighbour.name加到broadcast.nodesReceived
                #broadcast.nodesReceived.append(neighbour.name)
                sendSocket.sendto(packet, (ipaddress, int(neighbour.portNum)))
        time.sleep(1)


def receive_thread():
    receiveSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)  # ipv4 and UDP
    receiveSocket.bind((ipaddress, int(currNode.portNum)))  # "" means localhost
    while True:
        try:
            message, senderAddress = receiveSocket.recvfrom(2048)
            message = pickle.loads(message)
            if (isinstance(message, broadcastMessage)):
                allNodes[message.sourceNode] = message.neighbours
                receiver_send_msg(message,socket)
                # 调用广播的function（在广播function里面加一个参数，参数就是广播包）
        except Exception:
            print("ERROR IN UDP RECEIVE")
            break

def receiver_send_msg(message,send_info_socket):

    #send_info=socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
    #msg=info(current_host_node.host_name)
    transit_into = pickle.dumps(message)
    while True:
        for neighbour_info in currNode.neighbours:
            if(neighbour_info.name not in message.nodesReceived ):
                message.nodesReceived.append(neighbour_info.name)
                send_info_socket.sendto(transit_into, (ipaddress, int(neighbour_info.portNum)))
        time.sleep(1)


def dijkstra_thread():
    #ID = currNode.nodeName
    while True:
        time.sleep(ROUTE_UPDATE_INTERVAL)
        # Dijkstra's algorithm
        Q = set(allNodes.keys())
        prev = dict()
        dist = dict()
        #d = {ID: 0}
        for node in allNodes.keys():
            dist[node] = 999
        dist[currNode.nodeName] = 0
        #S = set()
        while len(Q) != 0:
            u = minDistance(dist, Q)
            del dist[u]
            for neighbour in allNodes[u]:
                alt = dist[u] + neighbour.cost
                if alt < dist[v]:
                    dist[v] = alt
                    prev[v] = u

        '''print(f'I am Router {ID}')
        S.remove(ID)
        for i in sorted(S):
            u = i
            path = []
            while True:
                path.append(u)
                if u == ID:
                    break
                u = previous[u]
            path_string = ''.join([i for i in reversed(path)])
            print(f'Least cost path to router {i}:{path_string} and the cost: {round(d[i],1)}')
        sor = sorted(nodes_known.items(), key=lambda x:x[0])
        print(f'[Dijkstra] {sor}')'''


def minDistance(dist, sptSet):
    # Initilaize minimum distance for next node
    min = 9999
    # Search not nearest vertex not in the
    # shortest path tree
    for v in allNodes.keys():
        if dist[v] < min:
            min = dist[v]
            min_index = v
    return min_index


def initNode():
    f = open(sys.argv[1])
    line = f.readline()
    items = line.split(' ')
    nodeName = items[0]
    myPortNum = items[1]
    currNode = lsrNode(nodeName, myPortNum)
    line = f.readline()
    myNeighbourNum = line.strip()
    for i in range(int(myNeighbourNum)):
        line = f.readline()
        line = line.strip()
        items = line.split(' ')
        currNeighbour = lsrNeighbour(items[0], items[1], items[2])
        currNode.neighbours.append(currNeighbour)
    return currNode


def dijkstra():
    while True:
        time.sleep(2)
        for node in allNodes.keys():
            for neighbour in allNodes[node]:
                print(neighbour.name + " :   " + neighbour.cost)

currNode = initNode()
# print(len(currNode.neighbours))
t1 = threading.Thread(target=broadcast_thread)
t1.start()
t2 = threading.Thread(target=receive_thread)
t2.start()
t3 = threading.Thread(target=dijkstra)
t3.start()
'''while True:
    time.sleep(2)
    for node in allNodes.keys():
        for neighbour in allNodes[node]:
            print(neighbour.name + " :   " + neighbour.cost)'''
