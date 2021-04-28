package cse3040;

class Point{
	private double[] point;
	private int dimension;
	public Point(double[] point)
	{
		this.point=point;
		this.dimension=point.length;
	}
	public int getDimension()
	{
		return this.dimension;
	}
	public double getValue(int index)
	{
		return point[index];
	}
}
class EuclideanDistance{
	public static double getDist(Point a, Point b)
	{
		double distance=0;
		double sum=0;
		if(a.getDimension()!=b.getDimension())
			return -1.0;
		for(int i=0; i<a.getDimension(); i++)
		{
			sum+=(a.getValue(i)-b.getValue(i))*(a.getValue(i)-b.getValue(i));
		}
		distance=Math.sqrt(sum);
		return distance;
	}
}
class ManhattanDistance{
	public static double getDist(Point a, Point b)
	{
		double distance=0;
		if(a.getDimension()!=b.getDimension())
			return -1.0;
		for(int i=0; i<a.getDimension(); i++)
		{
			distance+=Math.abs(a.getValue(i)-b.getValue(i));
		}
		return distance;
	}
}
public class Problem09 {
	public static void main(String[] args) {
			Point p1 = new Point(new double[] {1.0, 2.0, 3.0});
			Point p2 = new Point(new double[] {4.0, 5.0, 6.0});
			System.out.println("Euclidean Distance: " + EuclideanDistance.getDist(p1, p2));
			System.out.println("Manhattan Distance: " + ManhattanDistance.getDist(p1, p2));
			Point p3 = new Point(new double[] {1.0, 2.0, 3.0});
			Point p4 = new Point(new double[] {4.0, 5.0});
			System.out.println("Euclidean Distance: " + EuclideanDistance.getDist(p3, p4));
			System.out.println("Manhattan Distance: " + ManhattanDistance.getDist(p3, p4));
	}
}
