library(reshape2)
library(ggplot2)

# Search through all of the CSV files in the current directory 
file.names <- dir(getwd(), pattern =".csv")

for(i in 1:length(file.names)) {
    # Import the data
    data = read.csv(file.names[i], header=TRUE, comment.char="#")

    # Get the file name without the extension
    file_sub = tools::file_path_sans_ext(file.names[i])
    # b_fft = fft(data$b)
    # b_fft_mag = sqrt(Im(data$b)^2 + Re(data$b)^2)
    # g_fft = fft(data$g)
    # g_fft_mag = sqrt(Im(data$g)^2 + Re(data$g)^2)
    # r_fft = fft(data$r)
    # r_fft_mag = sqrt(Im(data$r)^2 + Re(data$r)^2)

    b_scatter = ggplot(data, aes(x=t, y=b)) +
    	geom_line()

    g_scatter = ggplot(data, aes(x=t, y=g)) +
        geom_line()

    r_scatter = ggplot(data, aes(x=t, y=r)) +
        geom_line()

    # Save the plots
    png(filename=paste(file_sub ,"_b",".png", sep=""), width=800, height=600)
    print(b_scatter)
    dev.off()

    png(filename=paste(file_sub ,"_g",".png", sep=""), width=800, height=600)
    print(g_scatter)
    dev.off()

    png(filename=paste(file_sub ,"_r",".png", sep=""), width=800, height=600)
    print(r_scatter)
    dev.off()
}