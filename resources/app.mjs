import {runTaskTest, runClearTimeoutTest} from "./cases/tasks-test.mjs";
import {drawTextExample} from "./example/draw-text.js";

// runClearTimeoutTest();
// drawTextExample();

let a = new Canvas();

console.log(a);

console.log(a.sayHi());

a.setHi("modified hi");
console.log(a.sayHi());
console.log(a.sayHi());
let b = new Canvas();

console.log(b.sayHi());
console.log(a.sayHi());
console.log(a.sayHi());
console.log(a.sayHi());
console.log(b.sayHi());