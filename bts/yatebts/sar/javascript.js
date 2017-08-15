/** javascript.js: manual implementations of common javascript methods that 
  * are not natively available in yate's implementation.
  * 
  * Cellular Search and Rescue - Cellular Sensor BTS
  *   Copyright (C) 2017 Microsoft
  * 
  * This file is part of cell-sar/the Yate-BTS Project http://www.yatebts.com
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

Array.prototype.map = function(mapper) {
   var result = [];
   for (var i = 0; i < this.length; ++i) {
      result.push(mapper(this[i]));
   }
   return result;
};
