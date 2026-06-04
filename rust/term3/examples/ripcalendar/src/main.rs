use chrono::Datelike;
use colored::Colorize;

struct CalendarWidget {
    datetime: chrono::NaiveDateTime,
}

impl CalendarWidget {
    fn new() -> Self {
        let datetime = chrono::Local::now().naive_local();
        Self { datetime }
    }

    fn weekday_colored(&self, day: chrono::NaiveDate) -> colored::ColoredString {
        let source = day.day().to_string();
        let weekday = day.weekday().num_days_from_monday() as usize;
        let is_today = day == self.datetime.date();

        let colored_day = if weekday > 4 {
            source.red()
        } else {
            source.white()
        };

        if is_today {
            colored_day.on_green()
        } else {
            colored_day
        }
    }

    fn print_days(&self) {
        let month_start =
            chrono::NaiveDate::from_ymd_opt(self.datetime.year(), self.datetime.month(), 1)
                .unwrap();

        let mut weekday = chrono::Weekday::Mon;
        while weekday != month_start.weekday() {
            print!("   ");
            weekday = weekday.succ();
        }

        for day in month_start
            .iter_days()
            .take_while(|day| day.month() == month_start.month())
        {
            let colored_day = self.weekday_colored(day);
            print!("{: >3}", colored_day);
            if day.weekday() == chrono::Weekday::Sun {
                println!();
            }
        }

        println!();
    }

    fn print_time(&self) {
        println!("{: ^21}", self.datetime.format("%H:%M:%S"));
    }

    fn print_weekdays(&self) {
        let weekdays = ['M', 'T', 'W', 'T', 'F', 'S', 'S'];
        for day in weekdays {
            print!("{day: >3}");
        }
        println!();
    }

    fn print(&self) {
        self.print_weekdays();
        self.print_days();
        println!();
        self.print_time();
    }
}

fn main() {
    loop {
        std::process::Command::new("clear").status().unwrap();
        let calendar = CalendarWidget::new();
        calendar.print();
        std::thread::sleep(std::time::Duration::from_millis(1000));
    }
}
