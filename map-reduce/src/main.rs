use std::collections::HashMap;
use std::env;
use std::fs::{File, OpenOptions};
use std::io::{self, prelude::*};

fn count<'a>(contents: io::BufReader<File>) -> HashMap<String, isize> {
    let mut results = HashMap::new();

    contents
        .lines()
        .map(|line| line.unwrap())
        .flat_map(|line| {
            line.trim()
                .split(" ")
                .map(str::to_owned)
                .collect::<Vec<_>>()
        })
        .for_each(|word| {
            let count = results.entry(word).or_insert(0);
            *count += 1;
        });

    return results;
}

fn process_file(filename: String) -> Result<HashMap<String, isize>, String> {
    println!("File: {}", filename);

    let file = match File::open(filename) {
        Ok(file) => file,
        Err(msg) => return Err(format!("Error while opening file: {}", msg)),
    };

    let file_reader = io::BufReader::new(file);

    return Ok(count(file_reader));
}

fn save_results(filename: String, results: HashMap<String, isize>) -> Result<(), String> {
    println!("Results file: {}", filename);

    let mut result_file = match OpenOptions::new().create(true).write(true).open(filename) {
        Ok(file) => file,
        Err(msg) => return Err(format!("Error while opening file: {}", msg)),
    };

    let mut r: io::Result<usize> = Ok(0);

    for (word, count) in &results {
        let line = format!("{}: {}\n", word, count);
        r = r.or(result_file.write(line.as_bytes()));
    }

    return match r {
        Ok(_) => Ok(()),
        Err(_) => Err(String::from("Error while writing results")),
    };
}

fn main() {
    let mut args = env::args();

    //drop exec name
    args.next();

    let input_name = match args.next() {
        Some(arg) => arg,
        None => {
            println!("Didn't get a file name");
            return;
        }
    };

    let result_name = match args.next() {
        Some(arg) => arg,
        None => String::from("results.txt"),
    };

    process_file(input_name)
        .and_then(|results| save_results(result_name, results))
        .unwrap();
}
