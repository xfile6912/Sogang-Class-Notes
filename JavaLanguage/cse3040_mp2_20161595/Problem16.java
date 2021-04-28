package cse3040;


import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Iterator;

class Element implements Comparable<Element>{
	private String name;
	private double price;
	public Element(String name, double price) {
		this.name=name;
		this.price=price;
	}
	public String getName() {
		// TODO Auto-generated method stub
		return name;
	}
	public double getPrice() {
		// TODO Auto-generated method stub
		return price;
	}
	public String toString()
	{
		return name+" "+price;
	}
	public int compareTo(Element element) {
		// TODO Auto-generated method stub
		if(this.getPrice()==element.getPrice())
		{
			return this.getName().compareTo(element.getName());
		}
		else if(this.getPrice()<element.getPrice())
			return -1;
		else
			return 1;
	}
}

class ElementReader{
	public static ArrayList<Element> readElements(String filename)
	{
		BufferedReader file=null;
		ArrayList<Element> list=new ArrayList<Element>();
		try {
			file=new BufferedReader(new FileReader(filename));
			while(true)
			{
				String str=file.readLine();
				String name="";
				String price=""; 
				if(str==null)
					break;
				for(int i=0; i<str.length(); i++)
				{
					if(str.charAt(i)==' ')
					{
						name=str.substring(0, i);
						price=str.substring(i+1);
						break;
					}
				}
				Element element= new Element(name, Double.valueOf(price));
				list.add(element);//list¿¡ Ãß°¡.
			}
		} catch (FileNotFoundException e) {
			return null;
		} catch (IOException e) {
			return null;
		}finally {
			try {
				file.close();
			} catch (Exception e) {
				// TODO Auto-generated catch block
				return null;
			}
		}
		return list;
	}
}

public class Problem16 {
	public static void main(String args[]) {
		ArrayList<Element> list = ElementReader.readElements("input.txt");
		if(list == null) {
			System.out.println("Input file not found.");
			return;
		}
		Collections.sort(list);
		Iterator<Element> it = list.iterator();
		while(it.hasNext()) System.out.println(it.next());
	}
}

