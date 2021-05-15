/**
 * demo to test inspector debugging.
 */

function doNothing(a, b) {
   return a + b;
}
// runTaskTest();
while(true) {
   console.log("aaa");
   __w8__sleep();
   // __w8__sleep();
   // __w8__sleep();
   let a = 1 + 2;
   // __w8__sleep();
   let c = 2 + 3;
   for (let i = 1; i < 100; i++) {
       doNothing(a, c);
   }
   console.log(11233);
   __w8__poll();
}