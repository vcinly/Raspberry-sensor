#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#define MAX_TIME 85
#define GpioPort 7

//存储读取的数据
int dht11_val[5] = { 0, 0, 0, 0, 0 };
//此函数读取温度值
int dht11_read_val(){
	uint8_t lststate = HIGH;//HIGH在wiringPi.h定义为1
	uint8_t counter = 0;//计数器
	uint8_t j = 0, i;
	//初始化数组为0
	for (i = 0; i < 5; i++)
		dht11_val[i] = 0;
	//发送开始信号
	pinMode(GpioPort, OUTPUT);//设置GPIO为输出
	digitalWrite(GpioPort, LOW);//保持低电平至少18ms
	delay(18);
	digitalWrite(GpioPort, HIGH);//保持高电平20-40us
	delayMicroseconds(40);
	//开始接受dht响应信号
	pinMode(GpioPort, INPUT);//设置GPIO为输入
	//开始等待温度传感器回应（根据数据手册温度传感器回应时会拉低数据线）
	for (i = 0; i < MAX_TIME; i++)
	{
		counter = 0;
		while (digitalRead(GpioPort) == lststate){//如果此时的电平和当前电平相等说明电平没有发生变化，所以继续等下一次循环
			counter++;
			delayMicroseconds(1);
			if (counter == 255)//如果在循环了255+1次之后温度传感器还未变换数据线则退出此次循环
				break;
		}
		lststate = digitalRead(GpioPort);//读取数据线最新的状态值
		if (counter == 255)//如果在循环了255+1次之后温度传感器还未变换数据线则退出此次循环
			break;
		//前三次被忽略可能是在等温度传感器响应（这玩意不是数据手册上说的反应那么快的....）
		if ((i >= 4) && (i % 2 == 0)){//对2取余是因为偶数电平是用来传输数据的（从数据手册可以看出）
			dht11_val[j / 8] <<= 1;//左移一位（此时最后一位为0）
			if (counter > 16) //此处时间很长代表此处为1
				dht11_val[j / 8] |= 1; //将数据1写入
			j++;
		}
	}
	/*
	开始处理数据

	1.因为一次完整的数据输出为40位，所以j应大于40
	2.dht11_val[4]为校验和，此处判断校验和是否正确
	3.只打印dht11_val[0], dht11_val[2]是因为dht11_val[1], dht11_val[3]是小数部分，根据数据手册小数始终为零
	*/
	if ((j >= 40) && (dht11_val[4] == ((dht11_val[0] + dht11_val[1] + dht11_val[2] + dht11_val[3]) & 0xFF))){
		printf("RH:%d,TEMP:%d\n", dht11_val[0], dht11_val[2]);
		return 1;
	}
	else
		return 0;
}
int main(void){
	//重复5次进行尝试
	int attempts = 5;
	//初始化GPIO
	if (wiringPiSetup() == -1)
		exit(1);
	//进行5次尝试
	while (attempts){
		//获取温度值
		int success = dht11_read_val();
		//获取成功后退出
		if (success)
			break;
		attempts--;
		delay(2500);
	}
	return 0;
}
