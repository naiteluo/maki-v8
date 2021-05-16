// import {runTaskTest, runClearTimeoutTest} from "./cases/tasks-test.mjs";
// import {drawTextExample} from "./example/draw-text.js";
import {canvasApiTest} from "./cases/canvas-api-test.mjs";
// import {tryCatchTest} from "./cases/try-catch-test.js";

console.log("Enter app.mjs");

/**
 * opengl draw test
 */
// drawTextExample();

/**
 * canvas class test
 */
canvasApiTest();

/**
 * try catch test
 */
// tryCatchTest();

function wait() {
    setTimeout(() => {
        wait();
    }, 100);
}
wait();
// __w8__sleep();
