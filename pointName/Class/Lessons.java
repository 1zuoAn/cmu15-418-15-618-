package Class;

import java.util.ArrayList;

/**
 * @author 林子键
 * @version 1.0
 */
public class Lessons {
    private ArrayList<Course> courses = new ArrayList<>();

    public ArrayList<Course> getCourses() {
        return courses;
    }


    public Lessons(ArrayList<Course> courses) {
        this.courses = courses;
    }

    public Lessons() {
    }

    public void setCourses(ArrayList<Course> courses) {
        this.courses = courses;
    }

    public void addLesson(Course course) {
        getCourses().add(course);
    }

    public void addLessons(ArrayList<Course> courses) {
        getCourses().addAll(courses);
    }

    //添加单个课程
    public void initCourse(int num, String name, Students students) {
        this.addLesson(new Course(name, students.getStudents(), num));
    }


    //生成所有课程(可参考Students类中的initStudents方法,记得给lesson设置个名字)
    public void initCourses(int num, Students students) {
        ArrayList<Student> students1 = new ArrayList<>();
        ArrayList<Student> students2 = new ArrayList<>();
        ArrayList<Student> students3 = new ArrayList<>();
        ArrayList<Student> students4 = new ArrayList<>();
        ArrayList<Student> students5 = new ArrayList<>();
        for (int i = 0; i < 90; i++) {
            Student student1 = students.getStudents().get(i);
            Student student2 = students.getStudents().get(i+90);
            Student student3 = students.getStudents().get(i+180);
            Student student4 = students.getStudents().get(i+270);
            Student student5 = students.getStudents().get(i+360);
            students1.add(student1);
            students2.add(student2);
            students3.add(student3);
            students4.add(student4);
            students5.add(student5);
        }
        for (int i = 0; i < 90; i++) {
            students1.get(i).getRecords().put(new Course("高数",num),new ArrayList<>());
            students2.get(i).getRecords().put(new Course("英语",num),new ArrayList<>());
            students3.get(i).getRecords().put(new Course("大物",num),new ArrayList<>());
            students4.get(i).getRecords().put(new Course("导论",num),new ArrayList<>());
            students5.get(i).getRecords().put(new Course("算法",num),new ArrayList<>());
        }

        Course course1 = new Course("高数", students1, num);
        Course course2 = new Course("英语", students2, num);
        Course course3 = new Course("大物", students3, num);
        Course course4 = new Course("导论", students4, num);
        Course course5 = new Course("算法", students5, num);
        this.addLesson(course1);
        this.addLesson(course2);
        this.addLesson(course3);
        this.addLesson(course4);
        this.addLesson(course5);

    }


    //生成所有课程的一轮缺勤名单,并写入每个course和student的缺勤名单里(records)
    //可以用上Course底下的recordMis方法
    public void createAllMis(){
        ArrayList<Course> arrayList = new ArrayList<Course>();
        for(Course course : getCourses()){
            course.recordMis();
            arrayList.add(course);
        }
        setCourses(arrayList);
    }

    public boolean haveCourse(String courseName){
        return findCourse(courseName) != null;
    }

    public Course findCourse(String courseName){
        for(Course course : getCourses()){
            if(course.getName().equals(courseName)){
                return course;
            }
        }
        return null;
    }

    public ArrayList<Course> courseHaveStudent(Student student){
        ArrayList<Course> courses = new ArrayList<>();
        for (Course course : getCourses()){
            if(course.haveStudent(student)){
                courses.add(course);
            }
        }
        return courses;
    }

    public void initLesson(int num){
        this.getCourses().add(new Course("高数",num));
        this.getCourses().add(new Course("英语",num));
        this.getCourses().add(new Course("大物",num));
        this.getCourses().add(new Course("导论",num));
        this.getCourses().add(new Course("算法",num));
    }

}
