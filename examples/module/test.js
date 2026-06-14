import "/tt.js";

function output(s)
{
	debug(s);
}
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
