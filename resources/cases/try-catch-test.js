export function tryCatchTest() {
    console.log("ab");

    console.error("Error!!!!");

    throw new Error("create error");

    callNotExistFunc();
}