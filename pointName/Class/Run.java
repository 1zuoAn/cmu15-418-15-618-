package Class;

import java.sql.Connection;

/**
 * @author 林子键
 * @version 1.0
 */
public class Run {
    public static void main(String[] args) {
        Students students = Generate.generateStudents(450);
        Lessons lessons = Generate.generateLessons(20, students);
        Lessons pointLessons = new Lessons();
        Sample sample = new Sample();
        pointLessons.initLesson(20);
        Generate generate = new Generate();
        Connection con;
        Read read = new Read();

        try {
            con = read.createConnector("root","dax71205");
            read.execute(con, "CREATE TABLE RECORDS(" +
                    "    No char(20) primary key," +
                    "    name char(20) ," +
                    "    grade float(5,2) ," +
                    "    Course char(30) " +
                    ");");
            read.execute(con, "CREATE TABLE POINTRECORDS(" +
                    "    No char(20) not null primary key," +
                    "    name char(20) ," +
                    "    grade float(5,2) ," +
                    "    Course char(30) " +
                    ");");
        }catch (Exception e){
            System.out.println("false111");
        }

        for (int i = 0; i < 20; i++) {
            System.out.println("循环 " + i);
            Lessons lessons1 = generate.generateMis(lessons);
            generate.write(lessons1, i + 1, read);
            Lessons pointLessons1 = read.readDB(pointLessons, i + 1);
            sample.sampleAllPoint(lessons1, pointLessons1, i + 1);
            lessons = sample.getLessons();
            pointLessons = sample.getPointLessons();
            generate.pointWrite(pointLessons,i + 1,read);
            System.out.println("第"+ (i+1) + "次有效点名率为" + generate.generateE(pointLessons));
        }



    }
}
