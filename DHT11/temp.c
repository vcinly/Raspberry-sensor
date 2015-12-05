#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#define MAX_TIME 85
#define GpioPort 7

//�洢��ȡ������
int dht11_val[5] = { 0, 0, 0, 0, 0 };
//�˺�����ȡ�¶�ֵ
int dht11_read_val(){
	uint8_t lststate = HIGH;//HIGH��wiringPi.h����Ϊ1
	uint8_t counter = 0;//������
	uint8_t j = 0, i;
	//��ʼ������Ϊ0
	for (i = 0; i < 5; i++)
		dht11_val[i] = 0;
	//���Ϳ�ʼ�ź�
	pinMode(GpioPort, OUTPUT);//����GPIOΪ���
	digitalWrite(GpioPort, LOW);//���ֵ͵�ƽ����18ms
	delay(18);
	digitalWrite(GpioPort, HIGH);//���ָߵ�ƽ20-40us
	delayMicroseconds(40);
	//��ʼ����dht��Ӧ�ź�
	pinMode(GpioPort, INPUT);//����GPIOΪ����
	//��ʼ�ȴ��¶ȴ�������Ӧ�����������ֲ��¶ȴ�������Ӧʱ�����������ߣ�
	for (i = 0; i < MAX_TIME; i++)
	{
		counter = 0;
		while (digitalRead(GpioPort) == lststate){//�����ʱ�ĵ�ƽ�͵�ǰ��ƽ���˵����ƽû�з����仯�����Լ�������һ��ѭ��
			counter++;
			delayMicroseconds(1);
			if (counter == 255)//�����ѭ����255+1��֮���¶ȴ�������δ�任���������˳��˴�ѭ��
				break;
		}
		lststate = digitalRead(GpioPort);//��ȡ���������µ�״ֵ̬
		if (counter == 255)//�����ѭ����255+1��֮���¶ȴ�������δ�任���������˳��˴�ѭ��
			break;
		//ǰ���α����Կ������ڵ��¶ȴ�������Ӧ�������ⲻ�������ֲ���˵�ķ�Ӧ��ô���....��
		if ((i >= 4) && (i % 2 == 0)){//��2ȡ������Ϊż����ƽ�������������ݵģ��������ֲ���Կ�����
			dht11_val[j / 8] <<= 1;//����һλ����ʱ���һλΪ0��
			if (counter > 16) //�˴�ʱ��ܳ�����˴�Ϊ1
				dht11_val[j / 8] |= 1; //������1д��
			j++;
		}
	}
	/*
	��ʼ��������

	1.��Ϊһ���������������Ϊ40λ������jӦ����40
	2.dht11_val[4]ΪУ��ͣ��˴��ж�У����Ƿ���ȷ
	3.ֻ��ӡdht11_val[0], dht11_val[2]����Ϊdht11_val[1], dht11_val[3]��С�����֣����������ֲ�С��ʼ��Ϊ��
	*/
	if ((j >= 40) && (dht11_val[4] == ((dht11_val[0] + dht11_val[1] + dht11_val[2] + dht11_val[3]) & 0xFF))){
		printf("RH:%d,TEMP:%d\n", dht11_val[0], dht11_val[2]);
		return 1;
	}
	else
		return 0;
}
int main(void){
	//�ظ�5�ν��г���
	int attempts = 5;
	//��ʼ��GPIO
	if (wiringPiSetup() == -1)
		exit(1);
	//����5�γ���
	while (attempts){
		//��ȡ�¶�ֵ
		int success = dht11_read_val();
		//��ȡ�ɹ����˳�
		if (success)
			break;
		attempts--;
		delay(2500);
	}
	return 0;
}
