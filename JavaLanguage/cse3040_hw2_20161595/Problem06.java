package cse3040;

interface IntSequence{
	boolean hasNext();
	int next();
}
class FibonacciSequence implements IntSequence{
	int fn_2=0;
	int fn_1=1;
	int fn=0;
	int flag=0;
	public boolean hasNext() {
		//종료시켜줄 것인지
		return true;
	}

	@Override
	public int next() {
		if(flag==0)
		{
			flag=1;
			return fn_2;
		}
		else if(flag==1)
		{
			flag=2;
			return fn_1;
		}
		else
		{
			fn=fn_1+fn_2;
			fn_2=fn_1;
			fn_1=fn;
			return fn;
		}
	}
	
}

public class Problem06 {

	public static void main(String[] args) {
		IntSequence seq=new FibonacciSequence();
		for(int i=0; i<20; i++)
		{
			if(seq.hasNext()==false) break;
			System.out.print(seq.next()+" ");
		}
		System.out.println(" ");
	}
}
