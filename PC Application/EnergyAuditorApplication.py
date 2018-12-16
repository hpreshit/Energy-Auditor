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

        global table, counter
        counter = 0
        dynamodb_session = Session(aws_access_key_id=''.join(access_key_id),
                  aws_secret_access_key=''.join(secret_access_key),
                  region_name=''.join(region))

        dynamodb = dynamodb_session.resource('dynamodb')

        table = dynamodb.Table('energy_auditor_data_log')

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
        global dict_ts, dict_currentmA,  dict_powerW, dict_connected, dict_stale_read, dict_energy, data_dict, table, data
        self.ui.sensorStatus.setStyleSheet("background-color: rgb(0, 255, 0);")
        response = table.scan(Limit=200)
        # print("Count: ",response['Count'])
        datas = response['Items']
        data_dict = defaultdict(list)

        for data in datas:
            # print(data)
            data_dict[data['nodeId']].append((data['payload']['ts'],data['payload']['current'],data['payload']['connected'],data['payload']['stale_read'],data['payload']['btaddr']))

        dict_ts = {}
        dict_currentmA = {}
        dict_powerW = {}
        dict_connected = {}
        dict_stale_read = {}
        dict_energy = {}
        dict_nodeconnected = {}
        dict_latest_currentmA = {}
        dict_latest_ts = {}
        dict_latest_energy_WHr = {}
        dict_data_btaddr ={}

        latest_currentmA = []
        latest_ts = []
        latest_powermW = []
        data_connected = []
        data_btaddr = []

        for data['nodeId'] in data_dict.keys():
            data_value = data_dict[data['nodeId']]
            ts = []
            current_mA = []
            power_watt = []
            connect_status = []
            bt_addr = []

            for i in range(len(data_value)):
                data_value_element = data_value[i]
                if data_value_element[3] == 0:
                    ts.append(float(data_value_element[0]))
                    current_mA.append(float(data_value_element[1]))
                    connect_status.append(int(data_value_element[2]))
                    bt_addr.append(data_value_element[4])
                else:
                    ts.append(float(data_value_element[0]))
                    current_mA.append(0)
                    connect_status.append(int(data_value_element[2]))
                    bt_addr.append(data_value_element[4])

            for i in range(len(ts)):
                ts[i]=time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(float(ts[i])))

            dict_ts[data['nodeId']]=(ts)
            dict_currentmA[data['nodeId']]=(current_mA)
            # print(current_mA)
            power_watt = [i * (120/1000) for i in current_mA]
            # print(power_watt)
            dict_powerW[data['nodeId']]=(power_watt)
            # print( power_watt)
            energy_values = []
            for i in range(len( power_watt)):
                if i is 0:
                    energy_values.append( power_watt[i])
                else:
                    energy_values.append( power_watt[i]+ energy_values[i-1])

            # print(energy_values)
            dict_energy[data['nodeId']] = energy_values
            # latest_currentmA.append(current_mA[len(current_mA)-1])
            # latest_ts.append(ts[len(ts)-1])
            # latest_powermW.append(power_watt[len(power_watt)-1])
            # data_connected.append(connect_status[len(connect_status)-1])
            # data_btaddr.append(bt_addr[len(bt_addr)-1])
            dict_nodeconnected[data['nodeId']] = connect_status[len(connect_status)-1]
            dict_latest_currentmA[data['nodeId']] = current_mA[len(current_mA)-1]
            dict_latest_ts[data['nodeId']] = ts[len(ts)-1]
            dict_latest_energy_WHr[data['nodeId']] = round((energy_values[len(energy_values)-1]/120),2)
            dict_data_btaddr[data['nodeId']] = bt_addr[len(bt_addr)-1]
            # print(type(data['nodeId']))
            # print(latest_currentmA)

        keys_list = list(data_dict.keys())
        # print(keys_list[0])
        # print(keys_list[1])
        # print(len(data_dict.keys()))
        if len(data_dict.keys()) == 3:
            if dict_nodeconnected[keys_list[0]] == 1:
                self.ui.node1Status.setStyleSheet("background-color: rgb(0, 255, 0);")
                self.ui.node1AddressDisplay.setText(str(dict_data_btaddr[keys_list[0]]))
                self.ui.node1TimeDisplay.setText(str(dict_latest_ts[keys_list[0]]))
                self.ui.node1CurrentDisplay.setText(str(dict_latest_currentmA[keys_list[0]])+' mA')
                self.ui.node1PowerDisplay.setText(str(dict_latest_energy_WHr[keys_list[0]])+' Wh')

            if dict_nodeconnected[keys_list[1]] == 1:
                self.ui.node2Status.setStyleSheet("background-color: rgb(0, 255, 0);")
                self.ui.node2AddressDisplay.setText(str(dict_data_btaddr[keys_list[1]]))
                self.ui.node2TimeDisplay.setText(str(dict_latest_ts[keys_list[1]]))
                self.ui.node2CurrentDisplay.setText(str(dict_latest_currentmA[keys_list[1]])+' mA')
                self.ui.node2PowerDisplay.setText(str(dict_latest_energy_WHr[keys_list[1]])+' Wh')
            if dict_nodeconnected[keys_list[2]] == 1:
                self.ui.node3Status.setStyleSheet("background-color: rgb(0, 255, 0);")
                self.ui.node3AddressDisplay.setText(str(dict_data_btaddr[keys_list[2]]))
                self.ui.node3TimeDisplay.setText(str(dict_latest_ts[keys_list[2]]))
                self.ui.node3CurrentDisplay.setText(str(dict_latest_currentmA[keys_list[2]])+' mA')
                self.ui.node3PowerDisplay.setText(str(dict_latest_energy_WHr[keys_list[2]])+' Wh')
        elif len(data_dict.keys()) == 2:
            # print(dict_nodeconnected[keys_list[0]])
            if dict_nodeconnected[keys_list[0]] == 1:
                self.ui.node1Status.setStyleSheet("background-color: rgb(0, 255, 0);")
                self.ui.node1AddressDisplay.setText(str(dict_data_btaddr[keys_list[0]]))
                self.ui.node1TimeDisplay.setText(str(dict_latest_ts[keys_list[0]]))
                self.ui.node1CurrentDisplay.setText(str(dict_latest_currentmA[keys_list[0]])+' mA')
                self.ui.node1PowerDisplay.setText(str(dict_latest_energy_WHr[keys_list[0]])+' Wh')

            if dict_nodeconnected[keys_list[1]] == 1:
                self.ui.node2Status.setStyleSheet("background-color: rgb(0, 255, 0);")
                self.ui.node2AddressDisplay.setText(str(dict_data_btaddr[keys_list[1]]))
                self.ui.node2TimeDisplay.setText(str(dict_latest_ts[keys_list[1]]))
                self.ui.node2CurrentDisplay.setText(str(dict_latest_currentmA[keys_list[1]])+' mA')
                self.ui.node2PowerDisplay.setText(str(dict_latest_energy_WHr[keys_list[1]])+' Wh')
        else:
            if dict_nodeconnected[keys_list[0]] == 1:
                self.ui.node1Status.setStyleSheet("background-color: rgb(0, 255, 0);")
                self.ui.node1AddressDisplay.setText(str(dict_data_btaddr[keys_list[0]]))
                self.ui.node1TimeDisplay.setText(str(dict_latest_ts[keys_list[0]]))
                self.ui.node1CurrentDisplay.setText(str(dict_latest_currentmA[keys_list[0]])+' mA')
                self.ui.node1PowerDisplay.setText(str(dict_latest_energy_WHr[keys_list[0]])+' Wh')

    def plot_graph(self):
        global dict_ts, dict_currentmA,  dict_powerW, dict_connected, dict_stale_read, dict_energy, data_dict, data
        j=0
        length_keys = len(data_dict.keys())
        for data['nodeId'] in data_dict.keys():

            # plt.subplot(math.floor(length_keys/2.5),math.floor(length_keys/2.5),j+1)
            plt.subplot(3,1,j+1)
            # plt.subplot(1,1,j+1)
            dict_ts_values = list(dict_ts.values())
            start_time_list = dict_ts_values[0]
            start_time = start_time_list[0]
            end_time_list = dict_ts_values[len(dict_ts_values)-1]
            end_time = end_time_list[len(end_time_list)-1]
            plt.title('Current and Energy plot versus Number of Samples for Node ID: '+str(data['nodeId'])+'     '+'Start Time: '+ str(start_time) +'     '+'End Time: '+ str(end_time))
            plt.plot(range(len(dict_ts[data['nodeId']])), dict_currentmA[data['nodeId']], color='blue', label='Current in mA')
            plt.plot(range(len(dict_ts[data['nodeId']])),dict_energy[data['nodeId']], color='red', label='Energy consumption in watts')
            # plt.ylabel('Current in mA')
            plt.xlabel('Number of Samples')
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
