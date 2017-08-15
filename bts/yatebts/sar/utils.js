/** util.js: utility functions for the sar_lib engine
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

function log(text) {
   Engine.debug(Engine.DebugInfo, text);
}

var last_tmsi;
function createTmsi(nnsf_bits, nnsf_local_mask, nnsf_node_shift)
{
    var t = last_tmsi;
    if (t)
	    t = 1 * parseInt(t,16);
    else
	    t = 0;

    if (nnsf_bits > 0)
	    t = ((t & 0xff000000) >> nnsf_bits) | (t & nnsf_local_mask);
    t++;

    if (nnsf_bits > 0)
	    t = ((t << nnsf_bits) & 0xff000000) | nnsf_node_shift | (t & nnsf_local_mask);

    if ((t & 0xc0000000) === 0xc0000000)
	    t = nnsf_node_shift + 1;

    return last_tmsi = t.toString(16,8);
}

// convert a PDU number (swaps digit pairs)
function generatePduNumber(number) {
   var newNumber = "";
   for (var i = 1; i < number.length; ++i) {
      newNumber += number.charAt(i);
      newNumber += number.charAt(i - 1);
   }
   return newNumber;
}

// Generate a new Phone Number
function generatePhoneNumber() {
   var An = 2 + randomint(8);
   var A = An.toString();
   var Bn = randomint(10);
   var B = Bn.toString();
   var Cn = randomint(10);
   var C = Cn.toString();
   var Dn = randomint(10);
   var D = Dn.toString();
   var En = randomint(10);
   var E = En.toString();

   switch (randomint(25)) {
	// 4 digits in a row - There are 10,000 of each.
	case 0: return A+B+C+D+D+D+D;
	case 1: return A+B+C+C+C+C+D;
	case 2: return A+B+B+B+B+C+D;
	case 3: return A+A+A+A+B+C+D;
	// ABCCBA palidromes - There are about 10,000 of each.
	case 4: return A+B+C+C+B+A+D;
	case 5: return A+B+C+D+D+C+B;
	// ABCABC repeats - There are about 10,000 of each.
	case 6: return A+B+C+A+B+C+D;
	case 7: return A+B+C+D+B+C+D;
	case 8: return A+B+C+D+A+B+C;
	// AABBCC repeats - There are about 10,000 of each.
	case 9: return A+A+B+B+C+C+D;
	case 10: return A+B+B+C+C+D+D;
	// AAABBB repeats - About 1,000 of each.
	case 11: return A+A+A+B+B+B+C;
	case 12: return A+A+A+B+C+C+C;
	case 13: return A+B+B+B+C+C+C;
	// 4-digit straights - There are about 1,000 of each.
	case 14: return "2345"+B+C+D;
	case 15: return "3456"+B+C+D;
	case 16: return "4567"+B+C+D;
	case 17: return "5678"+B+C+D;
	case 18: return "6789"+B+C+D;
	case 19: return A+B+C+"1234";
	case 20: return A+B+C+"2345";
	case 21: return A+B+C+"3456";
	case 22: return A+B+C+"4567";
	case 23: return A+B+C+"5678";
	case 24: return A+B+C+"6789";
   }
}

function gcdNums(nums) {
   if (nums.length === 0)
      return null;
   if (nums.length === 1)
      return nums[0];

   var gcd = gcdTwoNums(nums[0], nums[1]);
   for (var i = 2; i < nums.length; ++i)
      gcd = gcdTwoNums(gcd, nums[i]);
   return gcd;
}

// calculate the GCD of 2 numbers with Euclid's Algorithm
function gcdTwoNums(a, b) {
   if (a < b) {
      var tmp = a;
      a = b;
      b = tmp;
   }
      
   var q = parseInt(a / b); // Math.floor doesn't exist...
   var r = a - q * b;

   if (r === 0) return b;
   return gcdTwoNums(b, r);
}
