export function drawTextExample() {

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
}
