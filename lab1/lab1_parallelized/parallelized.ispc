export void invert(uniform int8 inputChars[], uniform int8 outputChars[], uniform int w, uniform int32 i)
{
	foreach(j = 0 ... w - 1){
		outputChars[i*w+j] = 255 - inputChars[i*w+j];
	}
} 
