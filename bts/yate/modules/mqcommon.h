/** mqcommon.h: header file for a POSIX message queue to send messages 
  * to the OCP
  *    
  * Cellular Search and Rescue - Cellular Sensor BTS
  *   Copyright (C) 2017 Microsoft
  * 
  * This file is part of cell-sar
  * 
  * cell-sar is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 2 of the License, or
  * (at your option) any later version.
  * 
  * cell-sar is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  * 
  * You should have received a copy of the GNU General Public License
  * along with cell-sar.  If not, see <http://www.gnu.org/licenses/>.
  */


#include <mqueue.h>

#define QUEUE_NAME  "/sar"
#define MAX_SIZE    4096

#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: ", __func__, __LINE__); perror(#x); exit(-1); } } while (0)

class MQCommon
{
	public:
  		static void push(const char* buffer);
  		static char* pop();
  		static void cleanup();
	private:
		static void init(bool write);
		static mqd_t mq;
		static struct mq_attr attr;
		static char buffer[MAX_SIZE + 1];
};
