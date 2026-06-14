class Mutex {
let mutex;
function Mutex() {
	mutex = CreateNativeMutex();
}
function lock() {
	mutex.lock();
}
function unlock() {
	mutex.unlock();
}
function trylock() {
	return mutex.trylock();
}
};



class Runnable {
	let mutex;
	function Runnable(m) {
		mutex = m;
	}
	function run() {
		debug("Runnable1");
		mutex.lock();
		debug("Runnable2");
		mutex.unlock();
	}
};
let mutex = Mutex();
debug("main1");
mutex.lock();

let t = CreateNativeThread(NativeObject(Runnable(mutex)));
t.start();
sleep(1);
debug("main2");
sleep(1);
mutex.unlock();
sleep(1);
debug("main3");

t.wait();


