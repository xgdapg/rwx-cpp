i*2+!-3.as(1,-2,3-4,b[2-3*4]).aa

var a:vector<list<int>> = 3*(4-5)+1
var b = a
for var i = 0,j=0; i < 3; i+=1,i=2 {
	//print("i:"+i)
}
const b = a

fn func1<U,K>(v1: int, v2: float, v3: string, v4: bool) -> byte {
	var i = 0 // int
	if true && false || false {
		i = 1
	} else if v1 == 1 {
		i = 2
	} else {
		i = 3
	}
	println(i)
	var 今天="asdf
	asdf"
	var sss = "星\r\n期三"
	var fff = 0.11
	//var f = if v2 > 0 { 3 } else { 5 }
	println(v3, f)
	return
	return byte(1)
}
fn(){}

struct Point {
	x: int,
	y: &int,
	z:int
	a:int
}


impl Point<int> {
	//ctor() {
	//	self.x = 0
	//	self.y = 0
	//}
	//
	//ctor(x:int, y:int) {
	//	self.x = x
	//	self.y = y
	//}
	//
	//dtor() {
	//	println("free")
	//}

	fn print() {
		println(self)
	}
}

interface ToString {
	fn toString() -> string,
}

impl Point : ToString {
	fn toString() -> string {
		return "Point(x:" + self.x.toString() + ", y:" + self.y.toString() + ")"
	}
}

impl Point {
	fn add(p: Point) {
		self.x += p.x
		self.y += p.y
	}
}

interface OperatorAdd {
	fn add(other: T)
}

impl Point : OperatorAdd {
	fn add(p: Point) {
		self.add(p)
	}
	fn sub(){}
}

var localPoint = Point() // Point in local
var heapPoint = &Point() // Point in heap, &Point in local
var localPointRef = &localPoint // &Point to localPoint
var heapPointRef = &heapPoint // &Point to &&Point=&Point
/*
// eval(Point) == eval(&Point)
/*var p1: &Point = &localPoint*/
var p2: Point = p1.clone()
*/

var i: int // 0
var f: float // 0.0
var s: string // ""
var b: bool // false
var v: byte // 0
var a: byte[4] // [0,0,0,0]
//var tr: &{type} // null
var ar: &byte[] // null

//fn func<T>()