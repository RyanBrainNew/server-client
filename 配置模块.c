int getNum(char* s)
{
   if(!strcmp(s,"1")) return 1;  	//s和1相等 strcmp（s，1）为0 
									//!strcmp（s，1）为0 非0
									//if 判断为真 	return1
   if(!strcmp(s,"2")) return 2;     //s和2相等 strcmp（s，2）为0 
									//!strcmp（s，2）为0 非0
									//if 判断为真 	return2
   if(!strcmp(s,"3")) return 3;     //s和3相等 strcmp（s，3）为0 
									//!strcmp（s，3）为0 非0
									//if 判断为真	return3
   return -1;
}

int main()
{
    int count = 0;
	char temp;
	//接收缓存区
	char recv_buf[80];
	//比较数组
    char working[1];
	
	temp = recv_buf[0];
	working[0] = temp ;
	switch(getNum(working))
	{
		case 1:
			//保持原样
			break;
		case 2:
			//全部大写
			while(recv_buf[count])
			{
				toupper(recv_buf[count]);
				count++;
			}
			break;
		case 3:
			//全部小写
			while(recv_buf[count])
			{
				tolower(recv_buf[count]);
				count++;
			}
			break;
		default:
			//保持原样
			break;

	}
}