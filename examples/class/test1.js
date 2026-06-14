function output(s)
{
	debug(s);
}
class ClassA
{
   let className;
	let valueName = "InitA";
	function ClassA(param) {
      className = "ClassA";
  		 valueName = "[" + className + "-" + param + "|A|" + valueName + "]";
	}
	function ~ClassA() {
      debug("ClassA Done");
	}
   function displayClassA() {
       debug("ClassA:" + valueName);
   }
	function displayClass1() {
		displayClassA();
	}
	function displayClass() {
		displayClass1();
	}
};
class ClassB: ClassA
{
   let className;
	let valueName = "InitB";
	function ClassB(param):ClassA("from B=" + param) {
      className = "ClassB";
  		 valueName = className + "-" + param + "|B|" + valueName;
	}
	function ~ClassB() {
      debug("ClassB Done");
	}
	function displayClass1() {
		displayClassA();
	}
   function displayClassB() {
       debug("ClassB:" + valueName);
   }
};
debug("hello");
{
	let bObj = ClassB("Check");
	bObj.displayClass();
}
output("finish");
