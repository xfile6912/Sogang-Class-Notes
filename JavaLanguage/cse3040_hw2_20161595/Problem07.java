package cse3040;

import java.util.Scanner;

interface IntSequenceStr{
	boolean hasNext();
	int next();
}
class BinarySequenceStr implements IntSequenceStr{
	int num;
	int binary;
	int length=0;//binarySequence의 남은 length 저장
	public BinarySequenceStr(int num)
	{
		this.num=num;
		while(num!=0)
		{
			num=num/2;
			length++;
		}
	}
	@Override
	public boolean hasNext() {
		if(length!=0)
		{
			length--;
			return true;
		}
		return false;
	}
	@Override
	public int next() {
		int temp=(int)Math.pow(2, length);
		if(num>=temp)
		{
			num=num-temp;
			return 1;
		}
		return 0;
	}
}
public class Problem07 {
	 public static void main(String[] args) {
		 Scanner in = new Scanner(System.in);
		 System.out.print("Enter a positive integer: ");
		 String str = in.nextLine();
		 int num = Integer.parseInt(str);
		 in.close();
		 System.out.println("Integer: " + num);
		 IntSequenceStr seq = new BinarySequenceStr(num);
		 System.out.print("Binary number: ");
		 while(seq.hasNext()) System.out.print(seq.next());
		 System.out.println(" ");
		 }
}
