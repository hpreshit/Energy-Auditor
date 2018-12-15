#!/usr/bin/python3
import sys
from PyQt5 import QtWidgets, QtCore, QtGui
from PyQt5.QtWidgets import QDialog, QApplication

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

        global table
        dynamodb_session = Session(aws_access_key_id=''.join(access_key_id),
                  aws_secret_access_key=''.join(secret_access_key),
                  region_name=''.join(region))

        dynamodb = dynamodb_session.resource('dynamodb')

        table = dynamodb.Table('energy_auditor_data_log')

    def get_values(self):
        global dict_ts, dict_currentmA, dict_powermW, dict_connected, dict_stale_read, dict_energy, data_dict, table, data
        self.ui.sensorStatus.setStyleSheet("background-color: rgb(0, 255, 0);")
        response = table.scan(Limit=200)
        # print("Count: ",response['Count'])
        datas = response['Items']
        data_nodeId = []
        data_dict = defaultdict(list)

        for data in datas:
            # print(data)
            data_dict[data['nodeId']].append((data['payload']['ts'],data['payload']['current'],data['payload']['connected'],data['payload']['stale_read']))

        dict_ts = {}
        dict_currentmA = {}
        dict_powermW = {}
        dict_connected = {}
        dict_stale_read = {}
        dict_energy = {}
        latest_currentmA = []
        latest_ts = []
        latest_powermW = []
        data_connected = []

        for data['nodeId'] in data_dict.keys():
            data_nodeId.append(data['nodeId'])
            length_keys = len(data_dict.keys())
            data_value = data_dict[data['nodeId']]
            ts = []
            current_mA = []
            power_mW = []
            connect_status = []

            for i in range(len(data_value)):
                data_value_element = data_value[i]
                if data_value_element[3] == 0:
                    ts.append(float(data_value_element[0]))
                    current_mA.append(float(data_value_element[1]))
                    # connect_status.append(data_value_element[2])
                else:
                    ts.append(float(data_value_element[0]))
                    current_mA.append(0)
                    # connect_status.append(data_value_element[2])

            for i in range(len(ts)):
                ts[i]=time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(float(ts[i])))

            dict_ts[data['nodeId']]=(ts)
            dict_currentmA[data['nodeId']]=(current_mA)
            power_mW = [i * 120/1000 for i in current_mA]
            dict_powermW[data['nodeId']]=(power_mW)

            dict_powermW_values = dict_powermW[data['nodeId']]
            # print(dict_powermW_values)
            energy_values = []
            for i in range(len(dict_powermW_values)):
                if i is 0:
                    energy_values.append(dict_powermW_values[i])
                else:
                    energy_values.append(dict_powermW_values[i]+dict_powermW_values[i-1])

            dict_energy[data['nodeId']] = energy_values
            latest_currentmA.append(current_mA[len(current_mA)-1])
            latest_ts.append(ts[len(ts)-1])
            latest_powermW.append(power_mW[len(power_mW)-1])
            data_connected.append(connect_status[len(connect_status)-1])
            # print(type(data['nodeId']))

        self.ui.node1AddressDisplay.setText(data_nodeId[0])
        # self.ui.node2AddressDisplay.setText(data_nodeId[1])
        # self.ui.node3AddressDisplay.setText(data_nodeId[2])
        self.ui.node1TimeDisplay.setText(str(latest_ts[0]))
        # self.ui.node2TimeDisplay.setText(str(latest_ts[1]))
        # self.ui.node3TimeDisplay.setText(str(latest_ts[2]))
        self.ui.node1CurrentDisplay.setText(str(latest_currentmA[0]))
        # self.ui.node2CurrentDisplay.setText(str(latest_currentmA[1]))
        # self.ui.node3CurrentDisplay.setText(str(latest_currentmA[2]))
        self.ui.node1PowerDisplay.setText(str(latest_powermW[0]))
        # self.ui.node2PowerDisplay.setText(str(latest_powermW[1]))
        # self.ui.node3PowerDisplay.setText(str(latest_powermW[2]))
        # if data_connected[0] == 1:
        #     self.ui.node1Status.setStyleSheet("background-color: rgb(0, 255, 0);")
        # if data_connected[1] == 1:
        #     self.ui.node2Status.setStyleSheet("background-color: rgb(0, 255, 0);")
        # if data_connected[2] == 1:
        #     self.ui.node3Status.setStyleSheet("background-color: rgb(0, 255, 0);")

    def plot_graph(self):
        global dict_ts, dict_currentmA, dict_powermW, dict_connected, dict_stale_read, dict_energy, data_dict, data
        j=0
        for data['nodeId'] in data_dict.keys():

            # plt.subplot(math.floor(length_keys/2.5),math.floor(length_keys/2.5),j+1)
            plt.subplot(1,1,j+1)
            plt.title('Current and Energy plot versus TimeStamp for node Id '+str(data['nodeId']))
            plt.scatter(dict_ts[data['nodeId']], dict_currentmA[data['nodeId']], color='blue', label=data['nodeId'])
            plt.scatter(dict_ts[data['nodeId']],dict_energy[data['nodeId']], color='red', label='Energy consumption')
            plt.ylabel('Current in mA')
            plt.xlabel('TimeStamp')
            plt.legend()
            plt.tight_layout()
            j=j+1
        plt.show()

    def close(self):
        sys.exit(app.exec_())

if __name__ == "__main__":
    app = QtWidgets.QApplication(sys.argv)
    ui = Main()
    ui.show()
    sys.exit(app.exec_())
