import os, urlparse
import paho.mqtt.client as mqtt
import pymysql
import cgitb
from datetime import datetime


# CONEXAO COM O BANCO - DATABASE, USUARIO, SENHA E HOST
conn = pymysql.connect(
    db='ac',
    user='root',
    passwd='root',
    host='localhost')
c = conn.cursor()

cgitb.enable()

# CODIGOS DE CONSULTAS AO BANCO
    
# VERIFICA SE O RFID PASSADO EXISTE NO BANCO
# SE SIM, RETORNA UMA LISTA CONTENDO NOME E ID DO USUARIO CADASTRADO 
def consulta(num):
    retorno = {}
    retorno["userId"] = 0
    retorno["userName"] = ""
    sql = "SELECT id,name FROM User WHERE rfid = '%s'" % (num)
    
    c.execute(sql)
    
    r = c.fetchall()

    if len(r) > 0:
        retorno["userId"] = int(r[0][0])
        retorno["userName"] = r[0][1] + ""
        
    return retorno


# VERIFICA SE DADO USUARIO POSSUI REGISTRO ABERTO ASSOCIADO A SEU RFID
# CASO NAO HAJA, O HORARIO E REGISTRADO E TEM SEU SATUS DEFINIDO COMO ABERTO (1).
# CASO HAJA, O HORARIO E REGISTRADO E O STATUS DEFINIDO COMO FECHADO (0)

def registro(userData):
    try:
        sql_consulta = "SELECT id FROM History WHERE idUser = %i \
                        AND status = 1;"% (userData["userId"])
        c.execute(sql_consulta)
        r = c.fetchall()
        if len(r) > 0:
            timestamp = datetime.now()
            id_hist = r[0][0]
            sql_update = "UPDATE `History` SET `status` = 0, `saida` = '%s' \
                          WHERE id = %i;" % (timestamp,id_hist)
            #print sql_update
            c.execute(sql_update)
            conn.commit()
            return "SAINDO/" + userData["userName"]
        else:
            sql_insert = "INSERT INTO History (idUser,status) VALUES (%i,1);"\
                         % (userData["userId"])
            c.execute(sql_insert)
            conn.commit()
            return "ENTRANDO/" + userData["userName"]
            
    except:
        return "ERRO";
    
# SOBREESCREVEMOS O COMPORTAMENTO DE ALGUMAS
# FUNCOES PROPRIAS DO MQTT

# EXECUTADA QUANDO UMA NOVA CONEXAO E FEITA
def on_connect(mosq, obj, rc):
    print("rc: " + str(rc))

# EXECUTADA QUANDO UMA NOVA MENSAGEM E LIDA NA FILA
# PUBLICA NA FILA DE RESPOSTA SE O ACESSO FOI/NAO FOI LIBERADO
# + O NOME DO CADASTRADO PARA EXIBICAO NO LCD
def on_message(mosq, obj, msg):
    print(msg.topic + " " + str(msg.qos) + " " + str(msg.payload))

    cons = consulta(str(msg.payload))
    
    if(cons["userName"] != ""):
        retorno = registro(cons)
        print(retorno)
        mqttc.publish("retorno", retorno)
    else:
        mqttc.publish("retorno", "FALSE")
        
# EXECUTADO A CADA PUBLICACAO
def on_publish(mosq, obj, mid):
    print("Publish: " + str(mid))

# EXECUTADO A CADA FILA QUE UM SUBSCRIBE E DADO
def on_subscribe(mosq, obj, mid, granted_qos):
    print("Subscribed: " + str(mid) + " " + str(granted_qos))

# EXECUTADO EM CADA ESCRITA NO LOG
def on_log(mosq, obj, level, string):
    print(string)

# CRIACAO DO OBJETO DO TIPO mqtt.Client
mqttc = mqtt.Client()

# SOBRESCRITA DOS METODOS NATIVOS DO MQTT
mqttc.on_message = on_message
mqttc.on_connect = on_connect
mqttc.on_publish = on_publish
mqttc.on_subscribe = on_subscribe

# URL DO CLOUDMQTT E DA INSTANCIA AONDE AS FILAS ESTAO
# A URL DA INSTANCIA E COMPOSTA POR: mqtt://m12.cloudmqtt.com: + PORTA
# PORTA PODE SER ENCONTRADO NAS INFORMACOES DA INSTANCIA
url_str = os.environ.get('m12.cloudmqtt.com', 'mqtt://m12.cloudmqtt.com:15213')
url = urlparse.urlparse(url_str)

# ATRIBUICAO DO USUARIO COM ACESSO AS FILAS
# USER E PASSWORD PODEM SER ENCONTRADOS NAS INFORMACOES DA INSTANCIA
mqttc.username_pw_set("btqswnsx", "KpPhAf77eFeQ")
mqttc.connect(url.hostname, url.port)

mqttc.subscribe("acesso", 0)

mqttc.publish("acesso", "ae43d3b5")
rc = 0
while rc == 0:
    rc = mqttc.loop()
print("rc: " + str(rc))