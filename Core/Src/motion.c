#include "motion.h"
#include <stdio.h>
#include <stdlib.h>

PIT pit[PITITEM];
int absl(int num)
{
	if(num>=0)
		return num;
	else
		return -num;
}

void DampAutoPos(uint8_t channel)
{
	int i;
	float step;
	for(i=0;i<PITITEM;i++)
	{
		if(pit[i].channel == channel)
		{
			step = absl(pit[i].target-pit[i].current)/20+1;
			
			if(pit[i].target > pit[i].current+step)
			{
				pit[i].current += step;
				pit[i].match = False;
			}
			else if(pit[i].target < pit[i].current-step)
			{
				pit[i].current -= step;
				pit[i].match = False;
			}
			else if(absl(pit[i].target-pit[i].current)<=step)
			{
				pit[i].current = pit[i].target;
				pit[i].match = True;
			}
		}
	}
}

void DampManualPos(float step,uint8_t channel)
{
	int i;
	for(i=0;i<PITITEM;i++)
	{
		if(pit[i].channel == channel)
		{
			if(pit[i].target > pit[i].current+step)
			{
				pit[i].current += step;
				pit[i].match = False;
			}
			else if(pit[i].target < pit[i].current-step)
			{
				pit[i].current -= step;
				pit[i].match = False;
			}
			else if(absl(pit[i].target-pit[i].current)<=step)
			{
				pit[i].current = pit[i].target;
				pit[i].match = True;
			}
		}
	}
}

void DampCustormPos(uint16_t index,float step)
{
	if(pit[index].target > pit[index].current+step)
	{
		pit[index].current += step;
		pit[index].match = False;
	}
	else if(pit[index].target < pit[index].current-step)
	{
		pit[index].current -= step;
		pit[index].match = False;
	}
	else if(absl(pit[index].target-pit[index].current)<=step)
	{
		pit[index].current = pit[index].target;
		pit[index].match = True;
	}
}

void SetCurrent(uint8_t index,float value)
{
	pit[index].current = value;
}

void SetTarget(uint8_t index,float value)
{
	pit[index].target = value;
}

void SetChannel(uint8_t index,uint8_t channel)
{
	pit[index].channel = channel;
}
