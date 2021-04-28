package cse3040;
interface Shape{
	public double getArea();
}

class Circle implements Shape{
	private double r;//¹ÝÁö¸§
	public Circle(double r) {
		this.r=r;
	}
	@Override
	public double getArea() {
		// TODO Auto-generated method stub
		return r*r*Math.PI;
	}
	
}
class Square implements Shape{
	private double x;
	public Square(double x) {
		this.x=x;
	}
	@Override
	public double getArea() {
		// TODO Auto-generated method stub
		return x*x;
	}
	
}
class Rectangle implements Shape{
	private double width;
	private double height;
	public Rectangle(double width, double height) {
		this.width=width;
		this.height=height;
	}
	@Override
	public double getArea() {
		// TODO Auto-generated method stub
		return width*height;
	}
	
}

public class Problem08 {
	public static double sumArea(Shape[] arr)
	{
		double sum=0;
		for(int i=0; i<arr.length; i++)
		{
			sum+=arr[i].getArea();
		}
		return sum;
	}
	public static void main(String[] args) {
		Shape[] arr = { new Circle(5.0), new Square(4.0),
				new Rectangle(3.0, 4.0), new Square(5.0)};
				 System.out.println("Total area of the shapes is: " + sumArea(arr));
	}

}
