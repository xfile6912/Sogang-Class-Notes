package cse3040;

class Points{
	private double[] points;
	private double sum=0;
	public Points(double[] points)
	{
		this.points=points;
		for(int i=0; i<points.length; i++)
			sum+=this.points[i];
	}
	public double getSum()
	{
		return sum;
	}
	public String toString()
	{
		return "[sum of points: "+sum+"]";
	}
	public boolean equals(Object obj) {
		if(this==obj)
			return true;
		if(obj==null)
			return false;
		if(this.getClass()==obj.getClass())
		{
			Points points=(Points)obj;
			if(points!=null && sum==points.getSum())
				return true;
		}
		return false;
	}
	
}
public class Problem10 {
	public static void main(String[] args) {
		Points p1 = new Points(new double[] {1.0, 2.0, 3.0});
		Points p2 = new Points(new double[] {4.0, 5.0, 6.0});
		System.out.println(p1);
		System.out.println(p2);
		System.out.println(p1.equals(p2));
		Points p3 = new Points(new double[] {1.0, 4.0, 7.0});
		Points p4 = new Points(new double[] {3.0, 9.0});
		System.out.println(p3);
		System.out.println(p4);
		System.out.println(p3.equals(p4));
		Points p5 = new Points(new double[] {1.0, 2.0});
		Points p6 = null;
		System.out.println(p5);
		System.out.println(p6);
		System.out.println(p5.equals(p6));
		}
}
