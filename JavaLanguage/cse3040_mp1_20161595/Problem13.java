package cse3040;

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;

class Text{
	private int count[]=new int[26];//몇개들어있는지 저장함. 알파벳 [0]->a, [25]->z
	private String str;
	private BufferedReader file=null;
	public boolean readTextFromFile(String filename)
	{
		try {
			file=new BufferedReader(new FileReader(filename));
			for(int i=0; i<count.length; i++)//배열 초기화
				count[i]=0;
			while(true)
			{
				str=file.readLine();
				if(str==null)
					break;
				for(int i=0; i<str.length(); i++)//알파벳수만큼 숫자세줌.
				{
					if('A'<=str.charAt(i) && 'Z'>=str.charAt(i))//대문자
					{
						count[str.charAt(i)-'A']++;
					}
					else if('a'<=str.charAt(i)&&'z'>=str.charAt(i))//소문자
					{
						count[str.charAt(i)-'a']++;
					}
				}
			}
		} catch (FileNotFoundException e) {
			System.out.println("Input file not found.");
			return false;
		} catch (IOException e) {
			return false;
		}finally {
			try {
				file.close();
			} catch (Exception e) {
				// TODO Auto-generated catch block
				return false;
			}
		}
		return true;
	}
	public int countChar(char c)
	{
		return count[c-'a'];
	}
}

public class Problem13 {
	 public static void main(String[] args) {
		Text t = new Text();
	 	if(t.readTextFromFile("input_prob13.txt")) {
		 	for(char c = 'a'; c <= 'z'; c++) {
		 		System.out.println(c + ": " + t.countChar(c));
	 		}
	 	}
	 }
}

