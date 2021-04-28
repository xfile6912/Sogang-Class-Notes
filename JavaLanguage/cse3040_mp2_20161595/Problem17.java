package cse3040;

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.Iterator;
import java.util.Map;
import java.util.TreeMap;
import java.lang.String;
class ElementMap<T, V> extends TreeMap<T,V>
{
	public String toString()
	{
		String ret="";
		Iterator<Map.Entry<T,V>> it = this.entrySet().iterator();
		while(it.hasNext())
		{
			Map.Entry<T,V> entry=it.next();
			ret+=entry.getKey()+" "+entry.getValue()+"\n";
		}
		return ret;
	}
}

class MapManager{
	public static Map<String,Double> readData(String filename)
	{
		BufferedReader file=null;
		ElementMap<String, Double> map=new ElementMap<String, Double>();
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
				map.put(name, Double.valueOf(price));//map¿¡ Ãß°¡.
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
		return map;
	}
}


public class Problem17 {
	public static void main(String args[]) {
		Map<String, Double> map = MapManager.readData("input.txt");
		if (map == null) {
			System.out.println("Input file not found.");
			return;
		}
		System.out.println(map);
	}
}
