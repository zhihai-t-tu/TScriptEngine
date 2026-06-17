class Runnable {
	let mutex;
	function Runnable(m) {
		mutex = m;
	}
	function ~Runnable() {
		debug("Done2");
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
let tlist = Array();
{
let t = Thread(Runnable(mutex));
t.start();
sleep(1);
debug("main2");
mutex.unlock();
debug("main3");
tlist.push_back(t);
//t.wait();
}

sleep(2);
debug("main4");

