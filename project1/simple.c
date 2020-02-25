#include<stdio.h>
#include<math.h>

int main()	{
int arr[10];

printf("Input 10 elements \n");
int i;
for (i=0; i < 10; i++)
	{
	scanf("%d", &arr[i]);
	}
int sqroots = 0;
for (i = 0; i < 10; i++)
	{
	arr[i] = sqrt(arr[i]);
	sqroots += arr[i];	
	}

int result;
result = sqroots/10;


printf("The average of the square roots is %d \n", result);

return 0;

}
