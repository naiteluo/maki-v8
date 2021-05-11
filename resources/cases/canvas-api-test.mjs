export function canvasApiTest() {
    let a = new Canvas();
    console.log("canvas:", a);
    console.log(a.sayHi());
    a.setHi("new hi");
    console.log(a.sayHi());
}