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

let t = Thread(Runnable(mutex));
t.start();
sleep(1);
debug("main2");
sleep(1);
mutex.unlock();
sleep(1);
debug("main3");

t.wait();


