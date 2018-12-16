#!/usr/bin/python3
import sys
from PyQt5 import QtWidgets, QtCore, QtGui
from PyQt5.QtWidgets import QDialog, QApplication
from PyQt5.QtCore import QTimer,QTime,QThread,QEventLoop

from EnergyAuditor import Ui_EnergyAuditor

from collections import defaultdict

import boto3
from boto3 import dynamodb

from boto3.session import Session

import json
import matplotlib
import matplotlib.pyplot as plt
import time
import math

from config_aws import *

class Main(QDialog):
    def __init__(self):
        super(Main,self).__init__()
        self.ui = Ui_EnergyAuditor()
        self.ui.setupUi(self)


        self.ui.refreshButton.clicked.connect(self.get_values)
        self.ui.graphButton.clicked.connect(self.plot_graph)
        self.ui.exitButton.clicked.connect(self.close)
        self.ui.documentationLink.setOpenExternalLinks(True)
        self.ui.documentationLink.setText("<a href='https://github.com/hpreshit/Energy-Auditor'>Documentation Link</a>");

        global table1, counter, table2, keys_list
        keys_list = []
        previous_list = []
        initial=0
        counter = 0
        dynamodb_session = Session(aws_access_key_id=''.join(access_key_id),
                  aws_secret_access_key=''.join(secret_access_key),
                  region_name=''.join(region))

        dynamodb = dynamodb_session.resource('dynamodb')

        table1 = dynamodb.Table('energy_auditor_data_log')
        table2 = dynamodb.Table('energy_data_latest')

    def startTimer(self):
        self.timer1=QTimer()
        self.timer1.timeout.connect(self.get_values)
        self.timer1.start(120000)
        self.timer2=QTimer()
        self.timer2.timeout.connect(self.update_counter)
        self.timer2.start(1000)

    def update_counter(self):
        global counter
        counter = counter + 1
        self.ui.elapsedTime.display(counter)

    def get_values(self):
        self.startTimer()
        global dict_ts, dict_sorted_ts, dict_currentmA,  dict_powerW, dict_connected, dict_stale_read, dict_energy, data_dict_1, data_dict_2, table1, table2, data_1, data_2
        self.ui.sensorStatus.setStyleSheet("background-color: rgb(0, 255, 0);")
        response_table1 = table1.scan()
        response_table2 = table2.scan()
        # print("Count: ",response['Count'])
        datas_1 = response_table1['Items']
        datas_2 = response_table2['Items']
        data_dict_1 = defaultdict(list)
        data_dict_2 = defaultdict(list)

        for data_1 in datas_1:
            # print(data)
            data_dict_1[data_1['nodeId']].append((data_1['payload']['ts'],data_1['payload']['current'],data_1['payload']['connected'],data_1['payload']['node_id'],data_1['payload']['btaddr']))

        for data_2 in datas_2:
            # print(data)
            data_dict_2[data_2['nodeId']].append((data_2['payload']['ts'],data_2['payload']['current'],data_2['payload']['connected'],data_2['payload']['node_id'],data_2['payload']['btaddr']))

        dict_ts = {}
        dict_sorted_ts = {}
        dict_currentmA = {}
        dict_powerW = {}
        dict_connected = {}
        dict_stale_read = {}
        dict_energy = {}
        dict_nodeconnected = {}
        dict_node_number = {}
        dict_latest_currentmA = {}
        dict_latest_ts = {}
        dict_latest_energy_WHr = {}
        dict_data_btaddr ={}

        latest_currentmA = []
        latest_ts = []
        latest_powermW = []
        data_connected = []
        data_btaddr = []

        for data_1['nodeId'] in data_dict_1.keys():
            data_value = data_dict_1[data_1['nodeId']]
            # print(data_value)
            ts = []
            sorted_ts = []
            current_mA = []
            power_watt = []
            connect_status = []
            bt_addr = []
            # print(data_value)
            for i in range(len(data_value)):
                data_value_element = data_value[i]
                if data_value_element[2] == 1:
                    ts.append(float(data_value_element[0]))
                    current_mA.append(float(data_value_element[1]))
                    connect_status.append(int(data_value_element[2]))
                    bt_addr.append(data_value_element[4])
                else:
                    ts.append(float(data_value_element[0]))
                    current_mA.append(0)
                    connect_status.append(int(data_value_element[2]))
                    bt_addr.append(data_value_element[4])
            # print(ts)
            sorted_ts = sorted(ts)
            for i in range(len(sorted_ts)):
                sorted_ts[i]=time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(float(sorted_ts[i])))

            # print(ts)
            for i in range(len(ts)):
                ts[i]=time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(float(ts[i])))

            dict_sorted_ts[data_1['nodeId']]=sorted_ts
            dict_ts[data_1['nodeId']]=(ts)
            dict_currentmA[data_1['nodeId']]=(current_mA)
            # print(current_mA)
            power_watt = [i * (120/1000) for i in current_mA]
            # print(power_watt)
            dict_powerW[data_1['nodeId']]=(power_watt)
            # print( power_watt)
            energy_values = []
            for i in range(len( power_watt)):
                if i is 0:
                    energy_values.append( power_watt[i])
                else:
                    energy_values.append( power_watt[i]+ energy_values[i-1])

            # print(current_mA)
            # print(energy_values)
            dict_energy[data_1['nodeId']] = energy_values
            # latest_currentmA.append(current_mA[len(current_mA)-1])
            # latest_ts.append(ts[len(ts)-1])
            # latest_powermW.append(power_watt[len(power_watt)-1])
            # data_connected.append(connect_status[len(connect_status)-1])
            # data_btaddr.append(bt_addr[len(bt_addr)-1])
            # dict_nodeconnected[data['nodeId']] = connect_status[len(connect_status)-1]
            # dict_latest_currentmA[data['nodeId']] = current_mA[len(current_mA)-1]
            # dict_latest_ts[data['nodeId']] = ts[len(ts)-1]
            # dict_latest_energy_WHr[data['nodeId']] = round((energy_values[len(energy_values)-1]/120),2)
            # dict_data_btaddr[data['nodeId']] = bt_addr[len(bt_addr)-1]
            # print(type(data['nodeId']))
            # print(latest_currentmA)

        for data_2['nodeId'] in data_dict_2.keys():
            data_value_2 = data_dict_2[data_2['nodeId']]
            data_value_number = data_value_2[0]
            # print(data_value_number)
            # data_value_number[0]=time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(float(data_value_number[0])))
            dict_latest_ts[data_2['nodeId']] = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(float(data_value_number[0])))
            dict_latest_currentmA[data_2['nodeId']] = float(data_value_number[1])
            dict_nodeconnected[data_2['nodeId']] = int(data_value_number[2])
            dict_node_number[data_2['nodeId']] = int(data_value_number[3])
            dict_data_btaddr[data_2['nodeId']] = data_value_number[4]
            dict_latest_energy_WHr[data_2['nodeId']] = float(data_value_number[1])*120/1000


        # present_list = tuple(data_dict_2.keys())
        # if len(present_list) != len(previous_list):
        #     if initial == 0:
        #         previous_list = present_list
        #         keys_list = present_list
        #         initial = initial + 1
        #     else:
        #         previous_list = keys_list
        #         keys_list = present_list

        keys_list = tuple(data_dict_2.keys())


        # last = keys_list
        # print(keys_list)
        # print(keys_list[0])
        # print(keys_list[1])
        # print(keys_list[2])
        # print(len(data_dict.keys()))
        if len(keys_list) == 3:
        # for data_2['nodeId'] in data_dict_2.keys():
            if dict_nodeconnected[keys_list[0]] == 1:
                self.ui.node1_Number.display(dict_node_number[keys_list[0]])
                self.ui.node1Status.setStyleSheet("background-color: rgb(0, 255, 0);")
                self.ui.node1AddressDisplay.setText(str(dict_data_btaddr[keys_list[0]]))
                self.ui.node1TimeDisplay.setText(str(dict_latest_ts[keys_list[0]]))
                self.ui.node1CurrentDisplay.setText(str(dict_latest_currentmA[keys_list[0]])+' mA')
                self.ui.node1PowerDisplay.setText(str(dict_latest_energy_WHr[keys_list[0]])+' Wh')
            else:
                self.ui.node1Status.setStyleSheet("background-color: rgb(255, 0, 0);")
                self.ui.node1AddressDisplay.setText('')
                self.ui.node1TimeDisplay.setText('')
                self.ui.node1CurrentDisplay.setText('')
                self.ui.node1PowerDisplay.setText('')


            if dict_nodeconnected[keys_list[1]] == 1:
                self.ui.node2_Number.display(dict_node_number[keys_list[1]])
                self.ui.node2Status.setStyleSheet("background-color: rgb(0, 255, 0);")
                self.ui.node2AddressDisplay.setText(str(dict_data_btaddr[keys_list[1]]))
                self.ui.node2TimeDisplay.setText(str(dict_latest_ts[keys_list[1]]))
                self.ui.node2CurrentDisplay.setText(str(dict_latest_currentmA[keys_list[1]])+' mA')
                self.ui.node2PowerDisplay.setText(str(dict_latest_energy_WHr[keys_list[1]])+' Wh')
            else:
                self.ui.node2Status.setStyleSheet("background-color: rgb(255, 0, 0);")
                self.ui.node2AddressDisplay.setText('')
                self.ui.node2TimeDisplay.setText('')
                self.ui.node2CurrentDisplay.setText('')
                self.ui.node2PowerDisplay.setText('')

            if dict_nodeconnected[keys_list[2]] == 1:
                self.ui.node3_Number.display(dict_node_number[keys_list[2]])
                self.ui.node3Status.setStyleSheet("background-color: rgb(0, 255, 0);")
                self.ui.node3AddressDisplay.setText(str(dict_data_btaddr[keys_list[2]]))
                self.ui.node3TimeDisplay.setText(str(dict_latest_ts[keys_list[2]]))
                self.ui.node3CurrentDisplay.setText(str(dict_latest_currentmA[keys_list[2]])+' mA')
                self.ui.node3PowerDisplay.setText(str(dict_latest_energy_WHr[keys_list[2]])+' Wh')
            else:
                self.ui.node3Status.setStyleSheet("background-color: rgb(255, 0, 0);")
                self.ui.node3AddressDisplay.setText('')
                self.ui.node3TimeDisplay.setText('')
                self.ui.node3CurrentDisplay.setText('')
                self.ui.node3PowerDisplay.setText('')
        elif len(data_dict.keys()) == 2:
            if dict_nodeconnected[keys_list[0]] == 1 and keys:
                self.ui.node1_Number.display(dict_node_number[keys_list[0]])
                self.ui.node1Status.setStyleSheet("background-color: rgb(0, 255, 0);")
                self.ui.node1AddressDisplay.setText(str(dict_data_btaddr[keys_list[0]]))
                self.ui.node1TimeDisplay.setText(str(dict_latest_ts[keys_list[0]]))
                self.ui.node1CurrentDisplay.setText(str(dict_latest_currentmA[keys_list[0]])+' mA')
                self.ui.node1PowerDisplay.setText(str(dict_latest_energy_WHr[keys_list[0]])+' Wh')
            else:
                self.ui.node1Status.setStyleSheet("background-color: rgb(255, 0, 0);")
                self.ui.node1AddressDisplay.setText('')
                self.ui.node1TimeDisplay.setText('')
                self.ui.node1CurrentDisplay.setText('')
                self.ui.node1PowerDisplay.setText('')


            if dict_nodeconnected[keys_list[1]] == 1:
                self.ui.node2_Number.display(dict_node_number[keys_list[1]])
                self.ui.node2Status.setStyleSheet("background-color: rgb(0, 255, 0);")
                self.ui.node2AddressDisplay.setText(str(dict_data_btaddr[keys_list[1]]))
                self.ui.node2TimeDisplay.setText(str(dict_latest_ts[keys_list[1]]))
                self.ui.node2CurrentDisplay.setText(str(dict_latest_currentmA[keys_list[1]])+' mA')
                self.ui.node2PowerDisplay.setText(str(dict_latest_energy_WHr[keys_list[1]])+' Wh')
            else:
                self.ui.node2Status.setStyleSheet("background-color: rgb(255, 0, 0);")
                self.ui.node2AddressDisplay.setText('')
                self.ui.node2TimeDisplay.setText('')
                self.ui.node2CurrentDisplay.setText('')
                self.ui.node2PowerDisplay.setText('')

        elif len(data_dict.keys()) == 1:
            if dict_nodeconnected[keys_list[0]] == 1 and keys:
                self.ui.node1_Number.display(dict_node_number[keys_list[0]])
                self.ui.node1Status.setStyleSheet("background-color: rgb(0, 255, 0);")
                self.ui.node1AddressDisplay.setText(str(dict_data_btaddr[keys_list[0]]))
                self.ui.node1TimeDisplay.setText(str(dict_latest_ts[keys_list[0]]))
                self.ui.node1CurrentDisplay.setText(str(dict_latest_currentmA[keys_list[0]])+' mA')
                self.ui.node1PowerDisplay.setText(str(dict_latest_energy_WHr[keys_list[0]])+' Wh')
            else:
                self.ui.node1Status.setStyleSheet("background-color: rgb(255, 0, 0);")
                self.ui.node1AddressDisplay.setText('')
                self.ui.node1TimeDisplay.setText('')
                self.ui.node1CurrentDisplay.setText('')
                self.ui.node1PowerDisplay.setText('')

    def plot_graph(self):
        global dict_ts, dict_sorted_ts, dict_currentmA,  dict_powerW, dict_connected, dict_stale_read, dict_energy, data_dict_1, data_1
        j=0
        length_keys = len(data_dict_1.keys())
        for data_1['nodeId'] in data_dict_1.keys():

            # plt.subplot(math.floor(length_keys/2.5),math.floor(length_keys/2.5),j+1)
            plt.subplot(3,1,j+1)
            # plt.subplot(1,1,j+1)
            dict_ts_values = list(dict_sorted_ts.values())
            start_time_list = dict_ts_values[0]
            start_time = start_time_list[0]
            end_time_list = dict_ts_values[len(dict_ts_values)-1]
            end_time = end_time_list[len(end_time_list)-1]
            plt.title('Current and Energy plot versus Number of Samples for Node ID: '+str(data_1['nodeId'])+'     '+'Start Time: '+ str(start_time) +'     '+'End Time: '+ str(end_time))
            plt.plot(range(len(dict_ts[data_1['nodeId']])), dict_currentmA[data_1['nodeId']], color='blue', label='Current in mA')
            plt.plot(range(len(dict_ts[data_1['nodeId']])),dict_energy[data_1['nodeId']], color='red', label='Energy consumption in watts')
            # plt.ylabel('Current in mA')
            plt.xlabel('Number of Samples')
            plt.grid()
            plt.legend()
            matplotlib.pyplot.subplots_adjust(left=0.125, bottom=0.1, right=0.9, top=0.9, wspace=0.2, hspace=0.5)
            # plt.tight_layout()
            j=j+1
        plt.show()

    def close(self):
        sys.exit(app.exec_())

if __name__ == "__main__":
    app = QtWidgets.QApplication(sys.argv)
    ui = Main()
    ui.show()
    sys.exit(app.exec_())
