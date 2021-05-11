export function drawTextExample(cb) {

    let y = 10;
    let counter = 0;

    function renderFrame() {
        if (cb) {
            cb(counter);
        }
        glClear();
        glText(
            'V8 + gl ' + Math.random() + '.',
            10,
            y,
            30
        );
        y += 10;
        y = y > 470 ? 10 : y;
        counter++;
        if (!glfwTick()) {
            return;
        }
        // loop draw
        setTimeout(renderFrame, 100);
    }

    renderFrame();
}
