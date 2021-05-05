log('js exec start.');
log('hello ' + `v${8}.`);

log('');
log('setTimeout Tests start:');

// setTimeout Test

setTimeout(() => {
    log('timeout 1 triggered.');
    setTimeout(() => {
        log('timeout 2 triggered.');
        setTimeout(() => {
            try {
                ee();
            } catch (e) {
                log(e);
            }
            log('timeout 3 triggered.');
        }, 1000);
        setTimeout(() => {
            log('timeout 4 triggered.');
        }, 5000);
    }, 2000);
}, 1000);

// render test

let y = 10;

function renderFrame() {
    glClear();
    glText(
        'V8 + gl ' + Math.random() + '.',
        10,
        y,
        30
    );
    y += 10;
    y = y > 470 ? 10 : y;
    if (!glfwTick()) {
        return;
    }
    // loop draw
    setTimeout(renderFrame, 100);
}

renderFrame();


// base script
function main() {
    runLoop();
    do {
        runLoop();
    } while (isLoopAlive());
    closeLoop();
}

main();